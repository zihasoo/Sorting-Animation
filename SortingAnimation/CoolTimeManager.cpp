#include "CoolTimeManager.h"

void CoolTimeMgr::setUpdateCool(int x) {
	UpdateCool = x;
}
bool CoolTimeMgr::ButtonCoolDown(int curTime) {
	static int lastButtonPressedTime = 0, PressCool = 200;
	if ((curTime - lastButtonPressedTime) > PressCool) {
		lastButtonPressedTime = curTime;
		return true;
	}
	return false;
}
bool CoolTimeMgr::ResetCoolDown(int curTime) {
	static int lastButtonPressedTime = 0, PressCool = 200;
	if ((curTime - lastButtonPressedTime) > PressCool) {
		lastButtonPressedTime = curTime;
		return true;
	}
	return false;
}
bool CoolTimeMgr::UpdateCoolDown(int curTime) {
	static int lastUpdatedTime = 0;
	if (curTime - lastUpdatedTime > UpdateCool) {
		lastUpdatedTime = curTime;
		return true;
	}
	return false;
}
int CoolTimeMgr::UpdateCool = 30;