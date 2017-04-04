#include "Common.hpp"
#include "Primitive.hpp"

namespace X
{
	namespace Detail
	{
		template <typename ContinuationT>
		struct RasterizerDetail
		{
			bool IsFrontFace(Triangle const& triangle)
			{
				f32V3 v0 = f32V3(triangle.v0.position.X(), triangle.v0.position.Y(), triangle.v0.position.Z()) / triangle.v0.position.W();
				f32V3 v1 = f32V3(triangle.v1.position.X(), triangle.v1.position.Y(), triangle.v1.position.Z()) / triangle.v1.position.W();
				f32V3 v2 = f32V3(triangle.v2.position.X(), triangle.v2.position.Y(), triangle.v2.position.Z()) / triangle.v2.position.W();
				f32V3 edge0 = v0 - v1;
				f32V3 edge1 = v0 - v2;
				f32V3 cross = Cross(edge0, edge1);
				return (cross.Z() < 0);
			}

			void DepthTestAndWrite(ConcreteTexture2D<f32>& depthBuffer,
				ContinuationT const& fragmentContinuation,
				AttributeOutputPackage const& fragmentInput, Point<u32, 2> sceenCoordinate)
			{
				if (fragmentInput.position.Z() <= depthBuffer.GetValue(0, sceenCoordinate))
				{
					depthBuffer.SetValue(0, sceenCoordinate, fragmentInput.position.Z());
					fragmentContinuation(fragmentInput, sceenCoordinate);
				}
			}

			void LineDDA(Size<u32, 2> resolution, ConcreteTexture2D<f32>& depthBuffer,
				ContinuationT const& fragmentContinuation,
				AttributeOutputPackage const& v0, AttributeOutputPackage const& v1)
			{
				f32 dx = v1.position.X() - v0.position.X();
				f32 dy = v1.position.Y() - v0.position.Y();
				f32 x = v0.position.X();
				f32 y = v0.position.Y();
				u32 stepCount = static_cast<u32>(std::max(std::abs(dx), std::abs(dy)));
				f32 xStep = dx / stepCount;
				f32 yStep = dy / stepCount;

				Vertex dv = v1.vertex * v1.position.W() - v0.vertex * v0.position.W(); // diff of attribute/z
				Vertex v = v0.vertex * v0.position.W();
				Vertex vStep = dv / f32(stepCount);

				f32 dInverseZ = v1.position.W() - v0.position.W(); // diff of 1/z (view space)
				f32 inverseZ = v0.position.W();
				f32 zStep = dInverseZ / stepCount;

				f32 dDepth = v1.position.Z() - v0.position.Z(); // diff z (NDC)
				f32 depth = v0.position.Z();
				f32 dStep = dDepth / stepCount;

				DepthTestAndWrite(depthBuffer, fragmentContinuation, AttributeOutputPackage(v * (1 / inverseZ), f32V4(x, y, depth, inverseZ)), Point<u32, 2>(static_cast<u32>(x), static_cast<u32>(y)));
				for (u32 k = 0; k < stepCount; ++k)
				{
					x += xStep;
					y += yStep;

					inverseZ += zStep;
					f32 z = 1 / inverseZ;

					v = v + vStep;

					depth += dStep;
					//if (x < resolution.X() && x >= 0 && y < resolution.Y() && y >= 0 && depth <= 1 && depth >= 0)
					if (depth <= 1)
					{
						assert(x < resolution.X() && x >= 0 && y < resolution.Y() && y >= 0 && depth >= 0);
						DepthTestAndWrite(depthBuffer, fragmentContinuation, AttributeOutputPackage(v * z, f32V4(x, y, depth, inverseZ)), Point<u32, 2>(static_cast<u32>(x), static_cast<u32>(y)));
					}
				}
			}

			AttributeOutputPackage PerspectiveLerp(AttributeOutputPackage const& a0, AttributeOutputPackage const& a1, f32 t)
			{
				// 1/z3 = lerp(1/z1, 1/z2, t), z is view space z
				f32V4 position = Lerp(a0.position, a1.position, t);
				f32 inverseZ = position.W();
				// a3/z3 = lerp(a1/z1, a2/z2, t)
				Vertex v = (1 / inverseZ) * Lerp(a0.vertex * a0.position.W(), a1.vertex * a1.position.W(), t);
				return AttributeOutputPackage(v, position);
			}


			AttributeOutputPackage PerspectiveLerp3InputOverZ(AttributeOutputPackage const& a0, AttributeOutputPackage const& a1, AttributeOutputPackage const& a2, f32 t0, f32 t1, f32 t2)
			{
				// w is view space z
				f32V4 position = Lerp3(a0.position, a1.position, a2.position, t0, t1, t2);
				f32 inverseZ = position.W();
				// a3/z3 = lerp(a1/z1, a2/z2, t)
				Vertex v = (1 / inverseZ) * Lerp3(a0.vertex, a1.vertex, a2.vertex, t0, t1, t2);
								
				return AttributeOutputPackage(v, position);
			}


			struct LineForScanLine
			{
				f32 a;
				f32 b;
				// y cannot be equal. i.e. horizontal line.
				LineForScanLine(f32V2 const& start, f32V2 const& end)
				{
					assert(start.Y() - end.Y() != 0);
					a = (start.X() - end.X()) / (start.Y() - end.Y());
					b = (a * -end.Y()) + end.X();
				}
				// return x
				f32 GetX(f32 y)
				{
					return a * y + b;
				}
			};

			s32 RoundUp(f32 value)
			{
				return static_cast<s32>(std::floor(value + 0.5f));
			}

			s32 RoundDown(f32 value)
			{
				return static_cast<s32>(std::floor(value + 0.5f) - 1);
			};

			/*
			*	p0		or	p1--p2
			*	|\			|  /
			*	| \			| /
			*	|  \		|/
			*	p1--p2		p0
			*/
			void DoScanLine(Size<u32, 2> resolution, AttributeOutputPackage const &p0, AttributeOutputPackage const &p1, AttributeOutputPackage const &p2, s32 yEnd, s32 yStart, ConcreteTexture2D<f32>& depthBuffer,
				ContinuationT const& fragmentContinuation)
			{
				if (yEnd < yStart)
				{
					return;
				}

				LineForScanLine leftLine(f32V2(p0.position.X(), p0.position.Y()), f32V2(p1.position.X(), p1.position.Y()));
				LineForScanLine rightLine(f32V2(p0.position.X(), p0.position.Y()), f32V2(p2.position.X(), p2.position.Y()));

				f32 deltaX0 = p0.position.X() - p1.position.X();
				f32 deltaX1 = p1.position.X() - p2.position.X();
				f32 deltaX2 = p2.position.X() - p0.position.X();

				f32 deltaY0 = p0.position.Y() - p1.position.Y();
				f32 deltaY1 = p1.position.Y() - p2.position.Y();
				f32 deltaY2 = p2.position.Y() - p0.position.Y();

				f32 denominator = deltaX1 * deltaY2 - deltaY1 * deltaX2;

				if (denominator == 0.0)
				{
					// degenerated triangle
					return;
				}

				AttributeOutputPackage v0OverZ(p0.vertex * p0.position.W(), p0.position);
				AttributeOutputPackage v1OverZ(p1.vertex * p1.position.W(), p1.position);
				AttributeOutputPackage v2OverZ(p2.vertex * p2.position.W(), p2.position);

				f32 leftXMin = std::min(p0.position.X(), p1.position.X());
				f32 leftXMax = std::max(p0.position.X(), p1.position.X());

				f32 rightXMin = std::min(p0.position.X(), p2.position.X());
				f32 rightXMax = std::max(p0.position.X(), p2.position.X());

				for (s32 y = std::max(yStart, 0); y <= std::min(yEnd, s32(resolution.Y() - 1)); ++y)
				{
					f32 lx = Clamp(leftLine.GetX(f32(y)), leftXMin, leftXMax);
					f32 rx = Clamp(rightLine.GetX(f32(y)), rightXMin, rightXMax);

					s32 xStart = RoundUp(lx);
					s32 xEnd = RoundDown(rx);

					for (s32 x = std::max(xStart, 0); x <= std::min(xEnd, s32(resolution.X() - 1)); ++x)
					{
						// calculate barycentric coordinates
						f32 t0 = (deltaY1 * (x - p2.position.X()) - deltaX1 * (y - p2.position.Y())) / denominator;
						f32 t1 = (deltaY2 * (x - p2.position.X()) - deltaX2 * (y - p2.position.Y())) / denominator;
						f32 t2 = 1.0f - t0 - t1;

						AttributeOutputPackage v(PerspectiveLerp3InputOverZ(v0OverZ, v1OverZ, v2OverZ, t0, t1, t2));
						if (v.position.Z() <= 1)
						{
							assert(v.position.Z() >= 0);
							DepthTestAndWrite(depthBuffer, fragmentContinuation, v, Point<u32, 2>(x, y));
						}

					}
				}
			}

			/*
			*	p1--p2
			*	|  /
			*	| /
			*	|/
			*	p0
			*
			*	p1.y == p2.y
			*	p1.x <= p2.x
			*	p0.y <= p1.y
			*/
			void ScanLineLowerTriangle(Size<u32, 2> resolution, ConcreteTexture2D<f32>& depthBuffer,
				ContinuationT const& fragmentContinuation,
				AttributeOutputPackage const& p0, AttributeOutputPackage const& p1, AttributeOutputPackage const& p2)
			{
				assert(p1.position.Y() == p2.position.Y());
				assert(p1.position.X() <= p2.position.X());
				assert(p0.position.Y() <= p1.position.Y());


				s32 yStart = static_cast<u32>(RoundUp(p0.position.Y()));
				s32 yEnd = static_cast<u32>(RoundDown(p1.position.Y()));

				if (p0.position.Y() == p1.position.Y())
				{
					return;
				}

				DoScanLine(resolution, p0, p1, p2, yEnd, yStart, depthBuffer, fragmentContinuation);
				return;
			}
			/*
			*	p0
			*	|\
			*	| \
			*	|  \
			*	p1--p2
			*
			*	p1.y == p2.y
			*	p1.x <= p2.x
			*	p0.y >= p1.y
			*/
			void ScanLineUpperTriangle(Size<u32, 2> resolution, ConcreteTexture2D<f32>& depthBuffer,
				ContinuationT const& fragmentContinuation,
				AttributeOutputPackage const& p0, AttributeOutputPackage const& p1, AttributeOutputPackage const& p2)
			{
				assert(p1.position.Y() == p2.position.Y());
				assert(p1.position.X() <= p2.position.X());
				assert(p0.position.Y() >= p1.position.Y());


				s32 yStart = static_cast<u32>(RoundUp(p1.position.Y()));
				s32 yEnd = static_cast<u32>(RoundDown(p0.position.Y()));

				if (p0.position.Y() == p1.position.Y())
				{
					return;
				}

				DoScanLine(resolution, p0, p1, p2, yEnd, yStart, depthBuffer, fragmentContinuation);
				return;
			}

			void TriangleScanLine(Size<u32, 2> resolution, ConcreteTexture2D<f32>& depthBuffer,
				ContinuationT const& fragmentContinuation, Triangle const& triangle)
			{
				AttributeOutputPackage const* pv0 = &triangle.v0;
				AttributeOutputPackage const* pv1 = &triangle.v1;
				AttributeOutputPackage const* pv2 = &triangle.v2;

				// make v0, v1, v2's y increase
				if (pv0->position.Y() > pv1->position.Y())
				{
					std::swap(pv0, pv1);
				}
				if (pv0->position.Y() > pv2->position.Y())
				{
					std::swap(pv0, pv2);
				}
				if (pv1->position.Y() > pv2->position.Y())
				{
					std::swap(pv1, pv2);
				}

				f32 t = (pv1->position.Y() - pv0->position.Y()) / (pv2->position.Y() - pv0->position.Y()); // t from v0 to v2
				AttributeOutputPackage v3 = PerspectiveLerp(*pv0, *pv2, t);
				assert(std::abs(v3.position.Y() - pv1->position.Y()) < std::abs(v3.position.Y() * 0.01)); // p should have same y with v1
				v3.position = f32V4(v3.position.X(), pv1->position.Y(), v3.position.Z(), v3.position.W());

				/*
				*    p2
				*    /|
				*   / |
				*  /  |
				* p1--p3
				*  \  |
				*   \ |
				*    \|
				*    p0
				*/
				if (pv1->position.X() < v3.position.X())
				{
					ScanLineLowerTriangle(resolution, depthBuffer, fragmentContinuation, *pv0, *pv1, v3);
					ScanLineUpperTriangle(resolution, depthBuffer, fragmentContinuation, *pv2, *pv1, v3);
				}
				/*
				* p2
				* |\
				* | \
				* |  \
				* p3--p1
				* |  /
				* | /
				* |/
				* p0
				*/
				else
				{
					ScanLineLowerTriangle(resolution, depthBuffer, fragmentContinuation, *pv0, v3, *pv1);
					ScanLineUpperTriangle(resolution, depthBuffer, fragmentContinuation, *pv2, v3, *pv1);
				}
			}

			/*
			*	z in [0, 1], w is 1 / input.w, (input.w is z in view space)
			*/
			f32V4 PerspectiveDivisionAndViewportTransform(Size<u32, 2> const& resolution, f32V4 const& input)
			{
				f32 inverseW = 1 / input.W();
				f32V4 v = input * f32V4(0.5f, 0.5f, 0.5f, 0.5f) * inverseW + f32V4(0.5f, 0.5f, 0.5f, 0.5f);
				return f32V4(v.X() * resolution.X(), v.Y() * resolution.Y(), v.Z(), inverseW);
			}

			enum ClipCode
			{
				None = 0x00,
				Left = 0x01,
				Right = 0x02,
				Bottom = 0x04,
				Top = 0x08,
				Near = 0x10,
				Far = 0x20,
			};

			ClipCode CalculateClipCode(f32V4 const& position)
			{
				u32 code = ClipCode::None;
				if (position.X() < -position.W())
				{
					code |= ClipCode::Left;
				}
				if (position.X() > position.W())
				{
					code |= ClipCode::Right;
				}
				if (position.Y() < -position.W())
				{
					code |= ClipCode::Bottom;
				}
				if (position.Y() > position.W())
				{
					code |= ClipCode::Top;
				}
				if (position.Z() < 0)
				{
					code |= ClipCode::Near;
				}
				if (position.Z() > position.W())
				{
					code |= ClipCode::Far;
				}
				return static_cast<ClipCode>(code);
			}

			void RasterizeLine(Size<u32, 2> resolution, ConcreteTexture2D<f32>& depthBuffer,
				ContinuationT const& fragmentContinuation, Triangle const& triangle)
			{
				f32V4 p0 = PerspectiveDivisionAndViewportTransform(resolution, triangle.v0.position);
				f32V4 p1 = PerspectiveDivisionAndViewportTransform(resolution, triangle.v1.position);
				f32V4 p2 = PerspectiveDivisionAndViewportTransform(resolution, triangle.v2.position);
				AttributeOutputPackage v0(triangle.v0.vertex, p0);
				AttributeOutputPackage v1(triangle.v1.vertex, p1);
				AttributeOutputPackage v2(triangle.v2.vertex, p2);

				LineDDA(resolution, depthBuffer, fragmentContinuation, v0, v1);
				LineDDA(resolution, depthBuffer, fragmentContinuation, v1, v2);
				LineDDA(resolution, depthBuffer, fragmentContinuation, v2, v0);
			}

			void RasterizeLine(Size<u32, 2> resolution, ConcreteTexture2D<f32>& depthBuffer,
				ContinuationT const& fragmentContinuation,
				AttributeOutputPackage const& v0, AttributeOutputPackage const& v1)
			{
				f32V4 p0 = PerspectiveDivisionAndViewportTransform(resolution, v0.position);
				f32V4 p1 = PerspectiveDivisionAndViewportTransform(resolution, v1.position);
				AttributeOutputPackage newV0(v0.vertex, p0);
				AttributeOutputPackage newV1(v1.vertex, p1);

				LineDDA(resolution, depthBuffer, fragmentContinuation, newV0, newV1);
			}

			// clip code calculation
			// reject triangles outside
			// clip triangles intersect near plan into more triangles
			void ClippingRasterizeLine(Size<u32, 2> resolution, ConcreteTexture2D<f32>& depthBuffer,
				ContinuationT const& fragmentContinuation, Triangle const& triangle)
			{
				if (!Detail::RasterizerDetail<ContinuationT>().IsFrontFace(triangle))
				{
					return;
				}

				ClipCode cc0 = CalculateClipCode(triangle.v0.position);
				ClipCode cc1 = CalculateClipCode(triangle.v1.position);
				ClipCode cc2 = CalculateClipCode(triangle.v2.position);
				if (cc0 == 0 && cc1 == 0 && cc2 == 0)
				{
					// inside, accept
					RasterizeLine(resolution, depthBuffer, fragmentContinuation, triangle);
				}
				else if ((cc0 & cc1 & cc2) > 0)
				{
					// outside all the planes, reject, nothing to do
				}
				else if (((cc0 | cc1 | cc2) & ClipCode::Near) != 0)
				{
					// clip with near plane
					AttributeOutputPackage* pv0 = &triangle.v0;
					AttributeOutputPackage* pv1 = &triangle.v1;
					AttributeOutputPackage* pv2 = &triangle.v2;
					// make v0, v1, v2's z increase
					if (pv0->position.Z() > pv1->position.Z())
					{
						std::swap(pv0, pv1);
					}
					if (pv0->position.Z() > pv2->position.Z())
					{
						std::swap(pv0, pv2);
					}
					if (pv1->position.Z() > pv2->position.Z())
					{
						std::swap(pv1, pv2);
					}

					if (pv1->position.Z() < 0)
					{
						// 2 vertices outside near
						/*
						* p2
						* |\
						* | \
						* |  \
						* -------- near(z=0)
						* |    p1
						* |   /
						* |  /
						* | /
						* |/
						* p0
						*/
						assert(pv0->position.Z() <= pv1->position.Z());
						assert(pv2->position.Z() >= 0);
						f32 t0 = (0 - pv2->position.Z()) / (pv0->position.Z() - pv2->position.Z());
						f32 t1 = (0 - pv2->position.Z()) / (pv1->position.Z() - pv2->position.Z());
						assert(0 <= t0 && t0 <= 1);
						assert(0 <= t1 && t1 <= 1);

						AttributeOutputPackage newP0(Lerp(pv2->vertex, pv0->vertex, t0), Lerp(pv2->position, pv0->position, t0));
						AttributeOutputPackage newP1(Lerp(pv2->vertex, pv1->vertex, t1), Lerp(pv2->position, pv1->position, t1));
						RasterizeLine(resolution, depthBuffer, fragmentContinuation, newP0, *pv2);
						RasterizeLine(resolution, depthBuffer, fragmentContinuation, newP1, *pv2);
					}
					else
					{
						// 1 vertex outside near
						/*
						* p2
						* |\
						* | \
						* |  \
						* |   \
						* |    p1
						* -------- near(z=0)
						* |  /
						* | /
						* |/
						* p0
						*/
						assert(pv1->position.Z() <= pv2->position.Z());
						assert(pv2->position.Z() >= 0);
						f32 t1 = (0 - pv1->position.Z()) / (pv0->position.Z() - pv1->position.Z());
						f32 t2 = (0 - pv2->position.Z()) / (pv0->position.Z() - pv2->position.Z());

						AttributeOutputPackage newP01(Lerp(pv1->vertex, pv0->vertex, t1), Lerp(pv1->position, pv0->position, t1));
						AttributeOutputPackage newP02(Lerp(pv2->vertex, pv0->vertex, t2), Lerp(pv2->position, pv0->position, t2));
						RasterizeLine(resolution, depthBuffer, fragmentContinuation, newP01, *pv1);
						RasterizeLine(resolution, depthBuffer, fragmentContinuation, newP02, *pv2);
						RasterizeLine(resolution, depthBuffer, fragmentContinuation, *pv1, *pv2);
					}
				}
				else
				{
					// intersect with other planes, accept
					RasterizeLine(resolution, depthBuffer, fragmentContinuation, triangle);
				}
			}

			void RasterizeTriangle(Size<u32, 2> resolution, ConcreteTexture2D<f32>& depthBuffer,
				ContinuationT const& fragmentContinuation, Triangle const& triangle)
			{

				f32V4 p0 = PerspectiveDivisionAndViewportTransform(resolution, triangle.v0.position);
				f32V4 p1 = PerspectiveDivisionAndViewportTransform(resolution, triangle.v1.position);
				f32V4 p2 = PerspectiveDivisionAndViewportTransform(resolution, triangle.v2.position);
				AttributeOutputPackage v0(triangle.v0.vertex, p0);
				AttributeOutputPackage v1(triangle.v1.vertex, p1);
				AttributeOutputPackage v2(triangle.v2.vertex, p2);
				Triangle newTriangle(v0, v1, v2);

				TriangleScanLine(resolution, depthBuffer, fragmentContinuation, newTriangle);
			}

			void NearClippingRasterize(Size<u32, 2> resolution, ConcreteTexture2D<f32>& depthBuffer,
				ContinuationT const& fragmentContinuation, Triangle const& triangle)
			{
				AttributeOutputPackage* pv0 = &triangle.v0;
				AttributeOutputPackage* pv1 = &triangle.v1;
				AttributeOutputPackage* pv2 = &triangle.v2;

				u32 outsideNearCount = 0;
				if (pv0->position.Z() < 0)
				{
					outsideNearCount += 1;
				}
				if (pv1->position.Z() < 0)
				{
					outsideNearCount += 1;
				}
				if (pv2->position.Z() < 0)
				{
					outsideNearCount += 1;
				}
				assert(outsideNearCount == 1 || outsideNearCount == 2);

				if (outsideNearCount == 1)
				{
					// 1 vertex outside near, make pv0 point to that vertex
					if (pv1->position.Z() < 0)
					{
						pv0 = &triangle.v1;
						pv1 = &triangle.v2;
						pv2 = &triangle.v0;
					}
					else if (pv2->position.Z() < 0)
					{
						pv0 = &triangle.v2;
						pv1 = &triangle.v0;
						pv2 = &triangle.v1;
					}
					else
					{
						// nothing to do
					}
					/*
					* p2---p1
					* |   /
					* -------- near(z=0)
					* | /
					* |/
					* p0
					*/
					assert(pv1->position.Z() >= 0);
					assert(pv2->position.Z() >= 0);
					f32 t1 = (0 - pv1->position.Z()) / (pv0->position.Z() - pv1->position.Z());
					f32 t2 = (0 - pv2->position.Z()) / (pv0->position.Z() - pv2->position.Z());

					AttributeOutputPackage newP01(Lerp(pv1->vertex, pv0->vertex, t1), Lerp(pv1->position, pv0->position, t1));
					AttributeOutputPackage newP02(Lerp(pv2->vertex, pv0->vertex, t2), Lerp(pv2->position, pv0->position, t2));
					Triangle newTriangle0(newP01, *pv1, newP02);
					Triangle newTriangle1(newP02, *pv1, *pv2);

					if (IsFrontFace(newTriangle0))
					{
						RasterizeTriangle(resolution, depthBuffer, fragmentContinuation, newTriangle0);
						RasterizeTriangle(resolution, depthBuffer, fragmentContinuation, newTriangle1);
					}
				}
				else
				{
					// 2 vertices outside near, make pv0 and pv1 point to these two
					if (pv0->position.Z() >= 0)
					{
						pv0 = &triangle.v1;
						pv1 = &triangle.v2;
						pv2 = &triangle.v0;
					}
					else if (pv1->position.Z() >= 0)
					{
						pv0 = &triangle.v2;
						pv1 = &triangle.v0;
						pv2 = &triangle.v1;
					}
					else
					{
						// nothing to do
					}
					/*
					* p2
					* |\
					* | \
					* -------- near(z=0)
					* |   \
					* p0---p1
					*/
					assert(pv2->position.Z() >= 0);
					f32 t0 = (0 - pv2->position.Z()) / (pv0->position.Z() - pv2->position.Z());
					f32 t1 = (0 - pv2->position.Z()) / (pv1->position.Z() - pv2->position.Z());
					assert(0 <= t0 && t0 <= 1);
					assert(0 <= t1 && t1 <= 1);

					AttributeOutputPackage newP0(Lerp(pv2->vertex, pv0->vertex, t0), Lerp(pv2->position, pv0->position, t0));
					AttributeOutputPackage newP1(Lerp(pv2->vertex, pv1->vertex, t1), Lerp(pv2->position, pv1->position, t1));
					Triangle newTriangle(newP0, newP1, *pv2);
					if (IsFrontFace(newTriangle))
					{
						RasterizeTriangle(resolution, depthBuffer, fragmentContinuation, newTriangle);
					}
				}

			}

			void ClippingRasterizeFill(Size<u32, 2> resolution, ConcreteTexture2D<f32>& depthBuffer,
				ContinuationT const& fragmentContinuation, Triangle const& triangle)
			{
				ClipCode cc0 = CalculateClipCode(triangle.v0.position);
				ClipCode cc1 = CalculateClipCode(triangle.v1.position);
				ClipCode cc2 = CalculateClipCode(triangle.v2.position);
				if ((cc0 | cc1 | cc2) == 0)
				{
					if (IsFrontFace(triangle))
					{
						// inside, accept
						RasterizeTriangle(resolution, depthBuffer, fragmentContinuation, triangle);
					}
				}
				else if ((cc0 & cc1 & cc2) > 0)
				{
					// outside all the planes, reject, nothing to do
				}
				else if (((cc0 | cc1 | cc2) & ClipCode::Near) != 0)
				{
					// clip with near plane
					NearClippingRasterize(resolution, depthBuffer, fragmentContinuation, triangle);
				}
				else
				{
					if (IsFrontFace(triangle))
					{
						// intersect with other planes, accept
						RasterizeTriangle(resolution, depthBuffer, fragmentContinuation, triangle);
					}
				}
			}
		};
	}

	template <typename ContinuationT>
	void LineRasterizer::Rasterize(Size<u32, 2> resolution, ConcreteTexture2D<f32>& depthBuffer,
		ContinuationT const& fragmentContinuation, Triangle const& triangle)
	{
		Detail::RasterizerDetail<ContinuationT>().ClippingRasterizeLine(resolution, depthBuffer, fragmentContinuation, triangle);
	}


	template <typename ContinuationT>
	void FillRasterizer::Rasterize(Size<u32, 2> resolution, ConcreteTexture2D<f32>& depthBuffer,
		ContinuationT const& fragmentContinuation, Triangle const& triangle)
	{
		Detail::RasterizerDetail<ContinuationT>().ClippingRasterizeFill(resolution, depthBuffer, fragmentContinuation, triangle);
	}
}