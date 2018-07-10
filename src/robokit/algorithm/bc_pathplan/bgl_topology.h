#ifndef _BGL_TOPOLOGY_H_
#define _BGL_TOPOLOGY_H_

#include <robokit/algorithm/bc_pathplan/cubic_bezier.h>
#include <robokit/foundation/utils/geometry.h>
#include <robokit/foundation/utils/pose_se2.h>
#include <boost/config.hpp>
#include <iostream>                         // for std::cout
#include <utility>                          // for std::pair
#include <algorithm>                        // for std::for_each
#include <boost/utility.hpp>                // for boost::tie
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

namespace rbk{
	namespace algorithm {
		class UPL {
		public:
			UPL(int sid = -1, int eid = -1, double t = 0.0f, bool forwards = true, std::string map = "")
				: start_id_(sid),
				end_id_(eid),
				percent_(t),
				forward_(forwards){}
			~UPL() {}
			int start_id_;
			int end_id_;
			bool forward_; // ��ʱ���ã���start_id_ -> end_id_��ʾ����
			double percent_;
		};

		// 外部图的结构
		struct EdgeProperty {
			int start_id_;
			int end_id_;
			double weight_;
			bool allow_backward_;
			rbk::foundation::GeoPoint ps_, pe_;
			rbk::foundation::GeoPoint cp0_, cp1_;
		};

		struct VertexProperty {
			int id_;
			bool allow_rotate_;
			rbk::foundation::GeoPoint p_;
			// id1 ->> id_ & id_ ->> id2 之间是锐角 则不能连通
			// std::map<std::pair<int, int>, bool> conn_;
		};

		// 对应 内部图, 边当做内部节点
		struct AbsNode {
			int start_id_;
			int end_id_;
			bool forward_;
			double weight_;
			int abs_id_;
		};

		//
		struct EdgeConn {
			int edgei_s_, edgei_e_;	// 外部节点id
			int edgeo_s_, edgeo_e_; // 有向
			bool c_;
		};

		/*
		* 新版本路径搜索
		*/
		class BGLTopology {
			// 外部图
			typedef boost::adjacency_list < boost::listS, boost::vecS, boost::bidirectionalS, VertexProperty, EdgeProperty > graph_e;
			// 内部图
			typedef boost::adjacency_list < boost::listS, boost::vecS, boost::directedS, boost::no_property, boost::property < boost::edge_weight_t, double > > graph_d;
			typedef boost::graph_traits < graph_d >::vertex_descriptor vertex_descriptor;
			typedef boost::graph_traits < graph_d >::edge_descriptor edge_descriptor;
			typedef std::pair<int, int> Edge;

		public:
			BGLTopology();
			~BGLTopology();
			void ResetMap();
			void InitMap();
			// 离Pos距离最近节点的id号
			int GetIdClosestNode(const rbk::foundation::GeoPoint& Pos);

			// 离Pos距离最近、且包含指定节点的边pair<int, int>
			std::pair<int, int> GetClosestEdge(const rbk::foundation::GeoPoint& Pos, int node_id);

			// 在所有边中查找离Pos距离最近的边pair<int, int>
			std::pair<int, int> GetClosestEdge(const rbk::foundation::GeoPoint& Pos);

			// 返回指定边参数
			bool GetEdge(std::pair<int, int> node_pair, NCubicBezier &e);

			// 主搜索,返回从当前到目标的边的列表
			std::vector<UPL> GetListTopologyEdge(UPL Pos, UPL Tar);

			// 韦磊用，重载计算路线长度及时间
			std::vector<UPL> GetListTopologyEdge(UPL Pos, UPL Tar, double &dist/*(m)*/, double &tt/*(sec)*/);
			void SetDefaultSpeed(double v/*(m/s)*/, double w/*(rad/s)*/);

			static const int D_dist_ = 300; 	/* 到达边的一个端点的阈值，小于会转到下一条边，单位 mm */

			// x, y,a (x/y单位与地图配置文件中单位一致, a 单位为弧度) 转成 UPL
			UPL xya2upl(double x, double y, double a);

			std::vector<unsigned int> getLandMarkIdList();
			void AddNode(int id, rbk::foundation::GeoPoint p, bool allow_rotate = true);
			void AddEdge(int start_id, int end_id, rbk::foundation::GeoPoint c0, rbk::foundation::GeoPoint c1, bool allow_backward = true);
		private:
			//mapmessage::AllMapInfo topology_info_;
			std::map<std::pair<int, int>, NCubicBezier> b_edges_;
			graph_e * ex_g_;	// 外部图
			std::vector<EdgeConn> conn_;
			double rotation_cost_;

			double default_v_;	// (mm/s)
			double default_w_;	// (deg/s)

		private:
			void PrintExGraph();
			void CalConnectivity();

			std::vector<AbsNode> find_upl(std::vector<AbsNode> &L, UPL p) {
				std::vector<AbsNode> res;
				AbsNode tmp;
				if (p.start_id_ == -1)
				{
					for (int i = 0 ; i < L.size() ;i++)
					{
						if (p.end_id_ == L[i].end_id_)
						{
							res.push_back(L[i]);
						}/*else if (p.end_id_ == L[i].start_id_)
						 {
						 res.push_back(L[i]);
						 }*/
					}
				}else{
					for(int i = 0; i < L.size(); ++i) {
						tmp = L[i];
						if (tmp.forward_) {
							if (p.start_id_ == tmp.start_id_ && p.end_id_ == tmp.end_id_)
								res.push_back(tmp);
						}
						else {
							if (p.start_id_ == tmp.end_id_ && p.end_id_ == tmp.start_id_)
								res.push_back(tmp);
						}
					}
				}
				return res;
			}

			std::vector<AbsNode> find_in_edge(std::vector<AbsNode> &L, int id) {
				std::vector<AbsNode> res;
				AbsNode tmp;
				for(int i = 0; i < L.size(); ++i) {
					tmp = L[i];
					if (tmp.end_id_ == id) {
						res.push_back(tmp);
					}
				}
				return res;
			}

			std::vector<AbsNode> find_out_edge(std::vector<AbsNode> &L, int id) {
				std::vector<AbsNode> res;
				AbsNode tmp;
				for(int i = 0; i < L.size(); ++i) {
					tmp = L[i];
					if (tmp.start_id_ == id) {
						res.push_back(tmp);
					}
				}
				return res;
			}

			AbsNode get_abs_id(std::vector<AbsNode> &L, int id) {
				AbsNode tmp;
				for(int i = 0; i < L.size(); ++i) {
					tmp = L[i];
					if (tmp.abs_id_ == id) {
						return tmp;
					}
				}
				tmp.abs_id_ = -1;
				return tmp;
			}
		};
	}
}

#endif
