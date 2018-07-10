#ifndef _SSL_SIMULATOR_H__
#define _SSL_SIMULATOR_H__

#include <robokit/core/rbk_core.h>
#include <server.h>

using namespace rbk;

class SSLSimulator : public NPluginInterface
{
public:
	typedef VisualInfoT VisualInfo;
	SSLSimulator();
	~SSLSimulator();
	void run();
	void loadFromConfigFile();
	void setSubscriberCallBack();
	void messageNavSpeedCallBack(google::protobuf::Message*);
	inline bool sendCommand(int num, short type, short value1, short value2, short value3);
	inline bool getVisualInfo(VisualInfo* info);
	inline bool MoveBall(double x, double y);
	inline bool SSLSimulator::Pause(int status);
};

RBK_INHERIT_PROVIDER(SSLSimulator, NPluginInterface, "1.0.0");

#endif // ~_SSL_SIMULATOR_H__