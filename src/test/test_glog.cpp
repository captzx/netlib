#include <glog/logging.h>

#include "common.h"

int main(int argc, char** argv) {
	google::InitGoogleLogging(argv[0]);
	FLAGS_log_dir = "./log";
	FLAGS_logtostderr = false;
	// google::SetLogDestination(google::GLOG_INFO, "./log/log");
	LOG(INFO) << "info test";//���һ��Info��־

	LOG(WARNING) << "warning test";//���һ��Warning��־

	LOG(ERROR) << "error test";//���һ��Error��־

	// LOG(FATAL) << "fatal test";//���һ��Fatal��־�����������ص���־�������֮�����ֹ����
	
	int num_cookies = 0;
	for (int i = 0; i < 14; ++i) {
		num_cookies++;
		LOG_IF(INFO, num_cookies > 10) << "Got lots of cookies";
		LOG_EVERY_N(INFO, 10) << "Got the " << google::COUNTER << "th cookie";//google::COUNTER ��¼����䱻ִ�д�������1��ʼ���ڵ�һ�����������־֮��ÿ�� 10 �������һ����־��Ϣ
	}
	google::ShutdownGoogleLogging();
	return 0;
}
