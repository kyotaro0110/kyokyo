#pragma once
#include "stdafx.h"

class SSHMonitoring {
private:
	String m_ip;
	double m_interval = 3.0;
	String m_lastResult = U"Waiting for data...";
	Stopwatch m_timer{ StartImmediately::Yes };

public:
	explicit SSHMonitoring(const String& ip, double interval = 3.0)
		: m_ip{ ip }
		, m_interval{ interval }
	{}

	void update() {
		if (m_timer.sF() >= m_interval) {
			const String command = U"ssh -o ConnectTimeout=2 kyokyo@{}"_fmt(m_ip) + U" \"uptime\"";
			ChildProcess child{ command, Pipe::StdOut };

			if (child) {
				std::string totalOutput;
				while (child.isRunning()) {
					totalOutput += child.read;
				}
			}
		}
	}
};
