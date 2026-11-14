#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <cmath>
#include "dobby.h"

#define OFFSET_ENTITY_LIST  0x0
#define OFFSET_LOCAL_PLAYER 0x0
#define OFFSET_HEAD_BONE    0x0
#define OFFSET_IS_DEAD      0x0
#define OFFSET_TEAM_ID      0x0
#define OFFSET_CAMERA_PITCH 0x0
#define OFFSET_CAMERA_YAW   0x0
#define OFFSET_PLAYER_POS   0x0
#define OFFSET_CAMERA_UPDATE 0x123456 // Need IDA Pro

struct Vector3 { float x, y, z; };
Vector3 operator-(Vector3 a, Vector3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
float Length(Vector3 v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

bool b_Aimbot = false;
float f_Smooth = 0.5f;

Vector3 GetHead(void* player) {
    void* headTrans = *(void**)((uintptr_t)player + OFFSET_HEAD_BONE);
    if (!headTrans) return {0,0,0};
    return *(Vector3*)((uintptr_t)headTrans + 0x44);
}

void* (*orig_CameraUpdate)(void* instance);
void* hook_CameraUpdate(void* instance) {
    if (instance && b_Aimbot) {
        void* localPlayer = *(void**)((uintptr_t)instance + OFFSET_LOCAL_PLAYER);
        if (localPlayer) {
            int myTeam = *(int*)((uintptr_t)localPlayer + OFFSET_TEAM_ID);
            void* entityList = *(void**)((uintptr_t)localPlayer + OFFSET_ENTITY_LIST);
            Vector3 myPos = *(Vector3*)((uintptr_t)localPlayer + OFFSET_PLAYER_POS);
            void* bestTarget = nullptr;
            float closestDist = 999999.0f;

            if (entityList) {
                for (int i = 0; i < 50; i++) {
                    void* enemy = *(void**)((uintptr_t)entityList + (i * 0x8));
                    if (!enemy) continue;
                    bool isDead = *(bool*)((uintptr_t)enemy + OFFSET_IS_DEAD);
                    int enemyTeam = *(int*)((uintptr_t)enemy + OFFSET_TEAM_ID);
                    if (!isDead && enemyTeam != myTeam) {
                        Vector3 headPos = GetHead(enemy);
                        float dist = Length(headPos - myPos);
                        if (dist < closestDist && dist < 300.0f) {
                            closestDist = dist;
                            bestTarget = enemy;
                        }
                    }
                }
            }
            if (bestTarget) {
                Vector3 headPos = GetHead(bestTarget);
                Vector3 dir = headPos - myPos;
                float pitch = -std::asin(dir.y / Length(dir)) * 57.2958f;
                float yaw = std::atan2(dir.x, dir.z) * 57.2958f;
                float currentPitch = *(float*)((uintptr_t)instance + OFFSET_CAMERA_PITCH);
                float currentYaw = *(float*)((uintptr_t)instance + OFFSET_CAMERA_YAW);
                *(float*)((uintptr_t)instance + OFFSET_CAMERA_PITCH) = currentPitch + (pitch - currentPitch) * f_Smooth;
                *(float*)((uintptr_t)instance + OFFSET_CAMERA_YAW) = currentYaw + (yaw - currentYaw) * f_Smooth;
            }
        }
    }
    return orig_CameraUpdate(instance);
}

JNIEXPORT void JNICALL Java_com_smith_ffhack_MainActivity_initHack(JNIEnv *env, jobject thiz) {
    void* handle = dlopen("libil2cpp.so", RTLD_LAZY);
    if (handle) {
        void* target = (void*)((uintptr_t)dlsym(handle, "il2cpp_init") + OFFSET_CAMERA_UPDATE);
        DobbyHook(target, (void*)hook_CameraUpdate, (void**)&orig_CameraUpdate);
    }
}

JNIEXPORT void JNICALL Java_com_smith_ffhack_MainActivity_setAimbot(JNIEnv *env, jobject thiz, jboolean enable) {
    b_Aimbot = enable;
}
