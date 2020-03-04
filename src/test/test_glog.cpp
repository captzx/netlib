#include <glog/logging.h>

#include "common.h"

int main(int argc, char** argv) {
	google::InitGoogleLogging(argv[0]);
	FLAGS_log_dir = "./log";
	FLAGS_logtostderr = false;
	// google::SetLogDestination(google::GLOG_INFO, "./log/log");
	LOG(INFO) << "info test";//输出一个Info日志

	LOG(WARNING) << "warning test";//输出一个Warning日志

	LOG(ERROR) << "error test";//输出一个Error日志

	// LOG(FATAL) << "fatal test";//输出一个Fatal日志，这是最严重的日志并且输出之后会中止程序
	
	int num_cookies = 0;
	for (int i = 0; i < 14; ++i) {
		num_cookies++;
		LOG_IF(INFO, num_cookies > 10) << "Got lots of cookies";
		LOG_EVERY_N(INFO, 10) << "Got the " << google::COUNTER << "th cookie";//google::COUNTER 记录该语句被执行次数，从1开始，在第一次运行输出日志之后，每隔 10 次再输出一次日志信息
	}
	google::ShutdownGoogleLogging();
	return 0;
}
