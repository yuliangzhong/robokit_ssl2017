#include "WorldModel.h"
/************************************************************************/
/* �����ռ�                                                             */
/************************************************************************/
extern bool IS_SIMULATION;
namespace {
	///> ��������
	// ���������ֵ
	const double Allow_Start_Kick_Dist = 5*Param::Vehicle::V2::PLAYER_SIZE;
	const double Allow_Start_Kick_Angle = 2.5 * Param::Vehicle::V2::KICK_ANGLE;
	
	// �ȶ�����������ž���
	const double beta = 1.0;
	const int critical_cycle = 10;
	int kick_last_cycle = 0;
	const int kick_stable_cnt[Param::Field::MAX_PLAYER + 1] = {2,2,2,2,2,2,2};     //С���ʱ��ҪСһ��
	int aim_count[Param::Field::MAX_PLAYER + 1] = {0,0,0,0,0,0,0};
	int aim_cycle[Param::Field::MAX_PLAYER + 1] = {0,0,0,0,0,0,0};
	// �жϸ�������: �����жϽǶ��Ƿ��е�����,����CMU���жϷ�ʽ
	bool deltaMarginKickingMetric(int current_cycle, double gt, double delta, double mydir, int myNum)
	{
		// �������ǰ�����ݣ���Ҫ�������
		if (current_cycle - aim_cycle[myNum] > critical_cycle) {
			aim_count[myNum] = 0;
		}
		aim_cycle[myNum] = current_cycle;

		// ���㵱ǰ��margin������¼
		double gl = Utils::Normalize(gt - delta);	// Ŀ�귽������ƫ�����ž�����ֵ
		double gr = Utils::Normalize(gt + delta);	// Ŀ�귽������ƫ�����ž�����ֵ
		double current_margin = max(Utils::Normalize(mydir - gl), Utils::Normalize(gr - mydir));

		// ����margin�ж��Ƿ�ﵽ���򾫶�
		bool kick_or_not = false;
		if (current_margin > 0 && current_margin < beta*2*delta) {
			if (aim_count[myNum]++ >= kick_stable_cnt[myNum]) {
				kick_or_not = true;
				aim_count[myNum] = kick_stable_cnt[myNum];
			}
		} else {
			aim_count[myNum] --;
			if (aim_count[myNum] < 0) {
				aim_count[myNum] = 0;
			}
		}

		return kick_or_not;
	}

	///> ���ڿ���
	// ���������ֵ
	const bool Allow_Start_Dribble = true;
	const double Allow_Start_Dribble_Dist = 6.0 * Param::Vehicle::V2::PLAYER_SIZE;
	const double Allow_Start_Dribble_Angle = 5.0 * Param::Vehicle::V2::KICK_ANGLE;

}

///> ��#�� ����ʱ�����Ƿ������λ��������
const bool  CWorldModel::KickDirArrived(int current_cycle, double kickdir, double kickdirprecision, int myNum) {
	static int last_cycle[Param::Field::MAX_PLAYER] = {-1,-1,-1,-1,-1,-1};
	static bool _dir_arrived[Param::Field::MAX_PLAYER] = {false,false,false,false,false,false};

	// ����
	kick_last_cycle = current_cycle;

	// ����
	//if (last_cycle[myNum-1] < current_cycle) {
		const PlayerVisionT& me = this->_pVision->OurPlayer(myNum);
		// ������ǰ �� ���� & �Ƕ�
		bool ball_in_front = true;	
		if (IS_SIMULATION){
			ball_in_front = (self2ballDist(current_cycle,myNum) < Allow_Start_Kick_Dist)
				&& (fabs(Utils::Normalize(self2ballDir(current_cycle,myNum)-me.Dir())) < Allow_Start_Kick_Angle);
		}

		
		// �Ƕ��Ƿ������
		//cout<<"ball_in_front "<<ball_in_front<<endl;
		bool my_dir_arrived = ball_in_front && ::deltaMarginKickingMetric(current_cycle,kickdir,kickdirprecision,me.Dir(),myNum);

		_dir_arrived[myNum-1] = /*ball_in_front &&*/ my_dir_arrived;
		last_cycle[myNum-1] = current_cycle;
	//}

	return _dir_arrived[myNum-1];
}