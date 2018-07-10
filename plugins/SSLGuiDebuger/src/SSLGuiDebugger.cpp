#include "SSLGuiDebugger.h"
#include "net_gui.h"
RBK_INHERIT_SOURCE(SSLGuiDebugger)
namespace{
	net_gdebug convert(const rbk::protocol::Debug_Msg&);
	net_gdebug convertFromArc(const rbk::protocol::Debug_Msg&);
	net_gdebug convertFromLine(const rbk::protocol::Debug_Msg&);
	net_gdebug convertFromText(const rbk::protocol::Debug_Msg&);
	net_gdebug convertFromRobot(const rbk::protocol::Debug_Msg&);
	net_gdebug convertFromCurve(const rbk::protocol::Debug_Msg&);
	net_gdebug convertFromPolygon(const rbk::protocol::Debug_Msg&);
}
SSLGuiDebugger::SSLGuiDebugger(void){
	send_debug_msg = true;
	gui_port = 20001;
	gui_bind_port = 20006;
	gui_socket.bind(gui_bind_port);
}

SSLGuiDebugger::~SSLGuiDebugger(void){
}

void SSLGuiDebugger::run(){
	static int count = 0;
	boost::chrono::time_point<boost::chrono::steady_clock> tp1;
	boost::chrono::time_point<boost::chrono::steady_clock> tp2;
	tp1 = boost::chrono::steady_clock::now();
	while(true){
		rbk::protocol::Debug_Msgs msgs;
		getSubscriberData(msgs);
		send(msgs);
		tp2 = boost::chrono::steady_clock::now();
		auto delt_time = boost::chrono::duration_cast<boost::chrono::milliseconds>((tp2 - tp1)).count();
		tp1 = tp2;
		//LogInfo(delt_time);
		if (delt_time < 1000.0/60) {
			Sleep(1000.0/60 - delt_time);	
		}
		//rbk::protocol::Message_TestMsg msg;
		//getSubscriberData(msg);
		//LogInfo(msg.count());
		//publishTopic(msgBack);
	}
}
void SSLGuiDebugger::loadFromConfigFile(){

}
void SSLGuiDebugger::setSubscriberCallBack(){
	setTopicCallBack<rbk::protocol::Debug_Msgs>();
	//createPublisher<rbk::protocol::Message_TestMsgBack>();
}

void SSLGuiDebugger::messageTestCallBack(google::protobuf::Message* msg){

} 

void SSLGuiDebugger::send(const rbk::protocol::Debug_Msgs& msgs){
	static int count = 0;
	count++;
	//if (send_debug_msg) {
		net_gdebugs new_msgs;
		new_msgs.totalnum = 0;
		new_msgs.totalsize = 0;
		memset(new_msgs.msg, 0, 500*sizeof(net_gdebug));
		int totalnum = msgs.msgs_size();
		if(totalnum>=500){
			LogWarn("!!!!!!!!!To Many Debug Message To Client!!!!!!!!!!!!!!!!!!!!!!!!");
			return;
		}
		for(int i=0;i<totalnum;i++){
			auto& msg = msgs.msgs(i);
			//net_gdebug new_msg;// = GDebugEngine::Instance()->get_queue_front();
			new_msgs.totalsize = count;//vision->Cycle();
			new_msgs.msg[new_msgs.totalnum] = ::convert(msg);
			new_msgs.totalnum++;
		}
		gui_socket.writeTo((char*)(&new_msgs),(8+sizeof(net_gdebug)*new_msgs.totalnum)/sizeof(char),"127.0.0.1",gui_port);
		//gui_socket.sendData(gui_port, (char*)(&new_msgs), 8+sizeof(net_gdebug)*new_msgs.totalnum, "127.0.0.1");
	//}
}
namespace{
	net_gdebug convert(const rbk::protocol::Debug_Msg& msg){
		net_gdebug ng;
		switch(msg.type()){
		case rbk::protocol::Debug_Msg_Debug_Type::Debug_Msg_Debug_Type_ARC:
			ng = convertFromArc(msg); break;
		case rbk::protocol::Debug_Msg_Debug_Type::Debug_Msg_Debug_Type_LINE:
			ng = convertFromLine(msg); break;
		case rbk::protocol::Debug_Msg_Debug_Type::Debug_Msg_Debug_Type_TEXT:
			ng = convertFromText(msg); break;
		case rbk::protocol::Debug_Msg_Debug_Type::Debug_Msg_Debug_Type_ROBOT:
			ng = convertFromRobot(msg); break;
		case rbk::protocol::Debug_Msg_Debug_Type::Debug_Msg_Debug_Type_CURVE:
			ng = convertFromCurve(msg); break;
		case rbk::protocol::Debug_Msg_Debug_Type::Debug_Msg_Debug_Type_POLYGON:
			ng = convertFromPolygon(msg); break;
		default:
			return net_gdebug();
		}
		ng.color = msg.color();
		ng.timestamp = 1;
		ng.robot_num = 0;
		ng.team = 0;
		return ng;
	}
	net_gdebug convertFromArc(const rbk::protocol::Debug_Msg& msg){
		auto arc = msg.arc();
		auto rec = arc.rectangle();
		net_gdebug ng;
		ng.msgtype = NET_GUI_DEBUG_ARC;
		ng.info.arc.center.valid = true;
		ng.info.arc.center.x = (rec.point1().x() + rec.point2().x())/2;
		ng.info.arc.center.y = (rec.point1().y() + rec.point2().x())/2;
		ng.info.arc.r = fabs(rec.point1().x() - rec.point1().x());
		ng.info.arc.a1 = arc.start();
		ng.info.arc.a2 = arc.end();
		ng.info.arc.flags = arc.fill() ? G_ARC_FILL : 0;
		return ng;
	}
	net_gdebug convertFromLine(const rbk::protocol::Debug_Msg& msg){
		auto line = msg.line();
		net_gdebug ng;
		ng.msgtype = NET_GUI_DEBUG_LINE;
		ng.info.line.p[0].valid = true;
		ng.info.line.p[0].x = line.start().x();
		ng.info.line.p[0].y = line.start().y();
		ng.info.line.p[1].valid = true;
		ng.info.line.p[1].x = line.end().x();
		ng.info.line.p[1].y = line.end().y();
		ng.info.line.flags = 0;
		return ng;
	}
	net_gdebug convertFromText(const rbk::protocol::Debug_Msg& msg){
		auto text = msg.text();
		net_gdebug ng;
		ng.msgtype = NET_GUI_DEBUG_FIELD_MSG;
		int textSize = text.text().size();
		if(textSize > 63) 
			textSize = 63;
		text.text().copy(ng.info.msg.msg,textSize);ng.info.msg.msg[textSize] = '\0';
		//printf("Size : %d ,Text : %s\n",textSize,ng.info.msg.msg);
		ng.info.msg.p.valid = true;
		ng.info.msg.p.x = text.pos().x();
		ng.info.msg.p.y = text.pos().y();
		return ng;
	}
	net_gdebug convertFromRobot(const rbk::protocol::Debug_Msg& msg){
		auto robot = msg.robot();
		net_gdebug ng;
		ng.msgtype = NET_GUI_DEBUG_ROBOT;
		ng.info.car.dir = robot.dir();
		ng.info.car.p.valid = true;
		ng.info.car.p.x = robot.pos().x();
		ng.info.car.p.y = robot.pos().y();
		return ng;
	}
	net_gdebug convertFromCurve(const rbk::protocol::Debug_Msg& msg){
		auto curve = msg.curve();
		net_gdebug ng;
		ng.msgtype = NET_GUI_DEBUG_CURVE;
		ng.info.curve.num = curve.num();
		ng.info.curve.minLimit = curve.minlimit();
		ng.info.curve.maxLimit = curve.maxlimit();
		return ng;
	}
	net_gdebug convertFromPolygon(const rbk::protocol::Debug_Msg& msg){
		auto tri = msg.polygon();
		net_gdebug ng;
		ng.msgtype = NET_GUI_DEBUG_TRIANGLE;
		if(msg.polygon().vertex_size() < 3) 
			return ng;
		for(int i=0;i<3;i++){
			ng.info.tri.p[i].valid = true;
			ng.info.tri.p[i].x = tri.vertex(i).x();
			ng.info.tri.p[i].y = tri.vertex(i).y();
		}
		return ng;
	}
}