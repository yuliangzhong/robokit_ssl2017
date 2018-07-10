#include "SSLRefereeBox.h"
//#include "playmode.h"
//#include <iostream>

RBK_INHERIT_SOURCE(SSLRefereeBox)

SSLRefereeBox::SSLRefereeBox()
{
	
}

SSLRefereeBox::~SSLRefereeBox() {
	
}

void SSLRefereeBox::loadFromConfigFile(){
	loadParam(m_referee_ip,"RefereeIP", "224.5.23.1");
	loadParam(m_referee_port, "RefereePort", 10003);
}

void SSLRefereeBox::setSubscriberCallBack(){
	createPublisher<SSL_Referee>();
}

void SSLRefereeBox::run(){
	_group_receiver.listen(m_referee_ip.c_str(), m_referee_port);
	while( true ) {
		SSL_Referee ssl_referee;
		char buffer[65536];
		int len = sizeof(buffer);
		int receive_byteCnt = _group_receiver.read(buffer, len);
		if (receive_byteCnt > 0) {
			ssl_referee.ParseFromArray(buffer, receive_byteCnt);
			publishTopic(ssl_referee);
		}
	}
}