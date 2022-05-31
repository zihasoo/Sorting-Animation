#pragma once

class CoolTimeMgr {
    static int UpdateCool;
public:
    static void setUpdateCool(int x);

    static bool ButtonCoolDown(int curTime);

    static bool ResetCoolDown(int curTime);

    static bool UpdateCoolDown(int curTime);
};
