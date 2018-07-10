#ifndef _ERROR_CODE_DEFINE_H_
#define _ERROR_CODE_DEFINE_H_

// Fatal 50000-51999
// --- internal
#define INTERNAL_ERROR            50000
#define RESOURCE_EXHAUSTION       50001
#define LUA_ERROR                 50002
// --- map
#define MAP_PARSE_ERROR           50100
#define MAP_LOAD_ERROR            50101
#define MAP_TOO_LARGE             50102
#define MAP_EMPTY                 50103
#define MAP_META_ERROR            50104
#define MAP_RESOLUTION_ILLEGAL    50105
#define MAP_FORMAT_INVALID        50106
// --- move skills
#define MOVESKILLS_PATH_ERROR     50200
#define MOVESKILLS_INIT_ERROR     50201
#define MOVESKILLS_REGISTER_ERROR 50202

// Error 52000-53999
// --- device
#define LASER_ERROR                    52100
#define LASER_DATA_INVALID             52101
#define IMU_ERROR                      52110
#define MOTOR_DRIVER_CONNECTION_ERROR  52111
#define ULTRASONIC_ERROR               52112
#define RFID_ERROR                     52113
#define MAGNETIC_0_TRACKER_ERROR       52114
#define MAGNETIC_1_TRACKER_ERROR       52115
#define CONTROLLER_NET_DOWN            52116
#define CONTROLLER_LINK_DOWN           52117
#define SUBSYSTEM_ERROR                52118
#define LOW_TASK_FREQUENCY             52119
#define MOTOR_GVDD_OVER_VOLTAGE        52130
#define MOTOR_FET_OVER_CURRENT         52131
#define MOTOR_OVER_TEMPERATURE         52132
#define MOTOR_VDD_UNDER_VOLTAGE        52133
#define MOTOR_ERROR_SEE_LOG            52135
// --- navigation
#define CAN_NOT_FIND_BEZIER_CURVE      52200
#define CAN_NOT_FIND_TARGET_ID         52201
#define PATH_PLAN_FAILED               52202
#define UNSUPPORTED_SKILL              52203
#define CHARGE_FAILED_WITHOUT_PREPOINT 52204
#define CAN_NOT_FIND_PATROL_ROUTE      52210
// --- localization
#define LOW_CONFIDENCE_OF_LOCALIZATION 52300

// Warning 54000-55999
// --- device
#define JOYSTICK_ERROR           54000
#define BATTERY_ERROR            54001
#define TEMPERATURE_SENSOR_ERROR 54002
#define MOTOR_OVERSPEED          54003
#define MOTOR_EMERGENCY_STOP     54004

// Notice 56000-57999
#define ODOMETER_JUMP              56000
#define FILE_SYSTEM_ERROR          57000
#define PARAM_SYSTEM_ERROR         57001
#define DATABASE_ERROR             57002
#define PLAY_AUDIO_ERROR           57003
#define GYRO_CALIBRATION_COMPLETED 57004
#define JSON_CONVERSION_ERROR      57005

#endif // ~_ERROR_CODE_DEFINE_H_
