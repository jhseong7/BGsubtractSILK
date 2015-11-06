#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <mmsystem.h> // need winmm.lib
#include <string>
#include <sstream>
#include <iomanip>

class FrameRate {
private:
	bool m_bTimerStarted, m_bUseMean;
	int m_nFrames, m_nCount, m_nDelay;
	float m_timeLast, m_timeCurrent;
	double m_fSum, m_fSquareSum;
	std::string m_strFrameRate;

public:
	FrameRate(int delay = 3) {
		if ( delay <= 0 )
			m_nDelay = 1, m_bUseMean = false;
		else
			m_nDelay = delay, m_bUseMean = true;
		m_bTimerStarted = false;
	}

	bool isTimeStarted() {return m_bTimerStarted;}
	std::string getFrameRateText() {return m_strFrameRate;}

	void startTimer() {
		m_bTimerStarted = true;
		m_nFrames = 0;
		m_nCount = - m_nDelay;
		m_timeCurrent = m_timeLast = 0.f;
		m_fSum = m_fSquareSum = 0.;
		m_strFrameRate = "Timing...";
		timeBeginPeriod(1);
	}
	void stopTimer() {
		m_bTimerStarted = false;
		timeEndPeriod(1);
	}

	bool updateFrameRate() {
		if ( ! m_bTimerStarted )
			return false;

		m_timeCurrent = timeGetTime() / 1000.f;
		m_nFrames ++;
		if ( m_timeCurrent - m_timeLast < 1.0 )
			return false;

		double fps = m_nFrames / (m_timeCurrent - m_timeLast);
		m_timeLast = m_timeCurrent;
		m_nFrames = 0;
		m_nCount ++;
		if ( m_nCount <= 0 )
			return true;

		m_fSum += fps;
		m_fSquareSum += fps * fps;
		double mean = m_fSum / m_nCount;
		double variance = sqrt((m_fSquareSum - m_fSum * mean) / m_nCount);

		std::ostringstream strbuff;
		if ( m_bUseMean )
			strbuff << "Current frame rate: " << std::setprecision(4) << mean << " fps";
		else
			strbuff << "Current frame rate: " << std::setprecision(4) << fps << " fps";
		m_strFrameRate = strbuff.str();

		return true;
	}
};
