#ifndef _RBK_MODEL_H_
#define _RBK_MODEL_H_

#include <boost/atomic/atomic.hpp>

#include <robokit/config.h>
#include <robokit/core/json.h>

namespace rbk {
    class RBK_API Model {
    public:
        enum Mode {
            UnknowMode = -1,
            Differential = 1
        };

        enum DriverType {
            UnknownDriver = -1,
            NoDriver = 0,
            Copley232 = 1,
            SeerDriverBoard = 2,
            CopleyCanOpen = 3,
            SeerDriverBoardHall = 4
        };

        enum BatteryInfoType {
            UnknownBatteryInfo = -1,
            NoBatteryInfo = 0,
            Paineng = 1,
            Modbus = 2
        };

        enum RFIDType {
            UnknownRFID = -1,
            NoRFID = 0,
            OrgRFID = 1
        };

        enum MagneticSensorType {
            UnknownMagneticSensor = -1,
            NoMagneticSensor = 0,
            HinSonMag = 1
        };

        enum CameraType {
            UnknownCamera = -1,
            NoCamera = 0
        };

        enum UltrasonicType {
            UnknownUltrasonic = -1,
            NoUltrasonic = 0,
            SeerUltrasonic = 1,
            KS136 = 2
        };

        enum Function {
            Nofunc = 0,
            Decelerate = 1,
            Stop = 2,
            Charge = 3
        };

        enum DeviceType {
            Nodevice = 0,
            Infrared = 1,
            Ultra = 2,
            CollisionSwitch = 3,
            Fallingdown = 4
        };

        enum DOFunction {
            NoDOFunc = 0,
            DOFuncAlarm = 1,
            DOFuncWaiting = 2,
            DOFuncRunning = 3,
            DOFuncPowering = 4
        };

        enum DOType {
            NoDOType = 0,
            DOTypeLight = 1,
            DOTypeMotor = 2,
            DOTypeDriver = 3,
            DOTypeLaser = 4
        };

        class Area {
        protected:
            uint8_t _index;
            std::vector<std::pair<double, double>> _vertex;
            std::vector<int> _limitedArea;
            uint8_t _vertexNum;
            uint32_t _workingMode;
            uint32_t _limitedAreaNum;

            friend Model;
        public:
            Area() : _index(0), _vertexNum(0), _workingMode(0), _limitedAreaNum(0) {}
            inline uint32_t index() const { return uint32_t(_index); }
            inline std::vector<std::pair<double, double>> vertex() const {
                return _vertex;
            }
            inline uint32_t vertexNum() const {
                return uint32_t(_vertexNum);
            }
            inline uint32_t workingMode() const {
                return uint32_t(_workingMode);
            }
            inline uint32_t limitedAreaNum() const {
                return uint32_t(_limitedAreaNum);
            }
            inline std::vector<int> limitedArea() const {
                return _limitedArea;
            }
        };

        class DecelerateArea : public Area {
        public:
            DecelerateArea() : Area() {
                _maxSpeed = -1;
            }
            inline double maxSpeed() const { return _maxSpeed; }
        protected:
            double _maxSpeed;
            friend Model;
        };

        class StopArea : public Area { };

        class SafeArea : public Area { };

        class Laser {
        private:
            uint8_t _index;
            std::string _model;       // 激光型号, SickLMS or HokuyoURG
            double _x;                // 在机器人坐标系的 x 偏移, m
            double _y;                // 在机器人坐标系的 y 偏移, m
            double _z;                // 在机器人坐标系的 z 偏移, m
            double _r;                // 在机器人坐标系的朝向角, 度
            double _aperture;         // 激光扫描范围角, 度
            double _realStep;         // 相邻激光束角度间隔, 度
            double _step;             // 实际使用的相邻激光束角度间隔, 度
            double _resolutionScale;  // 分辨率
            double _range;            // 激光有效距离
            double _minAngle;         // 使用的最小角度值
            double _maxAngle;         // 使用的最大角度值
            bool _upside;             // 激光是否正装
            std::string _ip;		  // 激光IP
            uint16_t  _port;          // 激光端口
            bool _useForLocalization; // 是否用来做定位
            friend Model;
        public:
            Laser() : _index(0), _model(""), _x(0), _y(0), _z(0), _r(0), _aperture(0),
                _realStep(0), _step(0), _resolutionScale(0), _minAngle(0),
                _maxAngle(0), _upside(false), _ip(""), _port(0), _useForLocalization(false) {}
            inline uint32_t index() const { return uint32_t(_index); }
            inline const std::string& model() const { return _model; }
            inline double x() const { return _x; }
            inline double y() const { return _y; }
            inline double z() const { return _z; }
            inline double r() const { return _r; }
            inline double aperture() const { return _aperture; }
            inline double realStep() const { return _realStep; }
            inline double step() const { return _step; }
            inline double resolutionScale() const { return _resolutionScale; }
            inline double range() const { return _range; }
            inline double minAngle() const { return _minAngle; }
            inline double maxAngle() const { return _maxAngle; }
            inline bool upside() const { return _upside; }
            inline std::string ip() const { return _ip; }
            inline uint16_t port() const { return _port; }
            inline bool useForLocalization() const { return _useForLocalization; }
        };

        class DI {
        private:
            uint8_t _index;
            uint8_t _id;
            Function _func;
            DeviceType _type;
            double _x;          // 在机器人坐标系的 x 偏移, m
            double _y;          // 在机器人坐标系的 y 偏移, m
            double _z;          // 在机器人坐标系的 z 偏移, m
            double _r;          // 在机器人坐标系的朝向角, 度
            bool _inverse;      // 是否逻辑反相
            double _maxSpeed;   // 当功能为减速时设置减速的速度大小 m/s
            friend Model;
        public:
            DI() : _index(0), _id(0), _func(Model::Nofunc), _type(Model::Nodevice), _x(0), _y(0), _z(0), _r(0),
                _inverse(false), _maxSpeed(-1) {}
            inline uint32_t index() const { return uint32_t(_index); }
            inline uint32_t id() const { return uint32_t(_id); }
            inline uint32_t func() const { return uint32_t(_func); }
            inline uint32_t type() const { return uint32_t(_type); }
            inline double x() const { return _x; }
            inline double y() const { return _y; }
            inline double z() const { return _z; }
            inline double r() const { return _r; }
            inline double inverse() const { return _inverse; }
            inline double maxSpeed() const { return _maxSpeed; }
        };

        class DO {
        private:
            uint8_t _index;
            uint8_t _id;
            DOFunction _func;
            DOType _type;
            double _x;          // 在机器人坐标系的 x 偏移, m
            double _y;          // 在机器人坐标系的 y 偏移, m
            double _z;          // 在机器人坐标系的 z 偏移, m
            double _r;          // 在机器人坐标系的朝向角, 度
            bool _inverse;      // 是否逻辑反相
            friend Model;
        public:
            DO() : _index(0), _id(0), _func(Model::NoDOFunc), _type(Model::NoDOType), _x(0), _y(0), _z(0), _r(0),
                _inverse(false) {}
            inline uint32_t index() const { return uint32_t(_index); }
            inline uint32_t id() const { return uint32_t(_id); }
            inline uint32_t func() const { return uint32_t(_func); }
            inline uint32_t type() const { return uint32_t(_type); }
            inline double x() const { return _x; }
            inline double y() const { return _y; }
            inline double z() const { return _z; }
            inline double r() const { return _r; }
            inline double inverse() const { return _inverse; }
        };

        class Ultrasonic {
        private:
            uint8_t _index;
            uint8_t _id;        // 位于超声接口的 id 号
            Function _func;     // 1 = 减速, 3 = 防跌落
            double _x;          // 在机器人坐标系的 x 偏移, m
            double _y;          // 在机器人坐标系的 y 偏移, m
            double _z;          // 在机器人坐标系的 z 偏移, m
            double _r;          // 在机器人坐标系的朝向角, 度
            double _minDist;    // 最小有效距离, m
            double _maxDist;    // 最大有效距离, m
            double _range;      // 探测范围角度, 度
            friend Model;
        public:
            Ultrasonic() : _index(0), _id(0), _func(Model::Nofunc), _x(0), _y(0), _z(0), _r(0),
                _minDist(0), _maxDist(0), _range(0) {}
            inline uint32_t index() const { return uint32_t(_index); }
            inline uint32_t id() const { return uint32_t(_id); }
            inline uint32_t func() const { return uint32_t(_func); }
            inline double x() const { return _x; }
            inline double y() const { return _y; }
            inline double z() const { return _z; }
            inline double r() const { return _r; }
            inline double minDist() const { return _minDist; }
            inline double maxDist() const { return _maxDist; }
            inline double range() const { return _range; }
        };

        class RFID {
        public:
            RFID() {}
        private:
            friend Model;
        };

        class MagneticSensor {
        private:
            uint8_t _index;
            uint8_t _id;
            uint32_t _resolution;
            double _x;
            double _y;
            double _z;
            double _r;
            double _step;
            friend Model;
        public:
            MagneticSensor() : _index(0), _id(0), _resolution(0), _x(0), _y(0), _z(0), _r(0), _step(0) {}
            inline uint32_t index() const { return uint32_t(_index); }
            inline uint32_t id() const { return uint32_t(_id); }
            inline uint32_t resolution() const { return _resolution; }
            inline double x() const { return _x; }
            inline double y() const { return _y; }
            inline double z() const { return _z; }
            inline double r() const { return _r; }
            inline double step() const { return _step; }
        };

        class Vision {
        public:
            Vision() {}
        private:
            friend Model;
        };

        class Speaker {
        public:
            Speaker() {}
        private:
            friend Model;
        };

    private:
        struct object_creator {
            // This constructor does nothing more than ensure that Instance()
            // is called before main() begins, thus creating the static
            // SingletonClass object before multithreading race issues can come up.
            object_creator() { Model::Instance(); }
            inline void do_nothing() const { }
        };

        static object_creator create_object;

        Model();

        ~Model();

        Model(const Model&);

        Model& operator=(const Model&);

    public:

        static Model* Instance();

        void init(const std::string& modelPath, const std::string& calibPath);

        void checkModel(const rapidjson::Document& modelDoc);

        void checkCalib(const rapidjson::Document& calibDoc);

        template<typename T>
        bool get(const std::string& pointer, T& value) const {
            if (!_init.load()) { return false; }
            return rbk::json::findByPointer(_model._modelDoc, pointer, value);
        }

        inline std::string name() const {
            return _model._name;
        }

        inline std::string version() const {
            return _model._version;
        }

        inline double width() const {
            return _model._width;
        }

        inline double head() const {
            return _model._head;
        }

        inline double tail() const {
            return _model._tail;
        }

        inline Mode mode() const {
            return _model._mode;
        }

        inline float wheelbase() const {
            return _model._wheelbase;
        }

        inline const std::vector<float>& wheelRadius() const {
            return _model._wheelRadius;
        }

        inline float reductionRatio() const {
            return _model._reductionRatio;
        }

        inline uint32_t encoderLine() const {
            return _model._encoderLine;
        }

        inline uint32_t maxMotorRpm() const {
            return _model._maxMotorRpm;
        }

        inline double inverse() const {
            return _model._inverse;
        }

        inline DriverType driver() const {
            return _model._driver;
        }

        inline uint32_t maxDINum() const {
            return _model._maxDINum;
        }

        inline uint32_t maxDONum() const {
            return _model._maxDONum;
        }

        inline uint32_t maxUltrasonicNum() const {
            return _model._maxUltrasonicNum;
        }

        inline RFIDType RFIDExists() const {
            return _model._RFIDExists;
        }

        inline MagneticSensorType magneticSensorExists() const {
            return _model._magneticSensorExists;
        }

        inline bool gyroExists() const {
            return _model._gyroExists;
        }

        inline bool autoGyroCal() const {
            return _model._autoGyroCal;
        }

        inline bool brakeExists() const {
            return _model._brakeExists;
        }

        inline bool autoBrake() const {
            return _model._autoBrake;
        }

        inline BatteryInfoType batteryInfoExists() const {
            return _model._batteryInfoExists;
        }

        inline bool LEDExists() const {
            return _model._LEDExists;
        }

        inline UltrasonicType ultrasonicExists() const {
            return _model._ultrasonicExists;
        }

        inline bool laserExists() const {
            return _model._laserExists;
        }

        inline CameraType cameraExists() const {
            return _model._cameraExists;
        }

        inline bool speakerExists() const {
            return _model._speakerExists;
        }

        inline bool useDecelerateArea() const {
            return _model._useDecelerateArea;
        }

        inline bool useStopArea() const {
            return _model._useStopArea;
        }

        inline bool useSafeArea() const {
            return _model._useSafeArea;
        }

        inline uint32_t decelerateAreaNum() const {
            return uint32_t(_model._decelerateAreaNum);
        }

        inline const DecelerateArea* decelerateArea(uint8_t index) const {
            if (index < _model._decelerateAreaNum) {
                return &(_model._decelerateAreas.at(index));
            }
            else {
                return nullptr;
            }
        }

        inline const std::vector<DecelerateArea>* decelerateAreas() const {
            return &(_model._decelerateAreas);
        }

        inline uint32_t stopAreaNum() const {
            return uint32_t(_model._stopAreaNum);
        }

        inline const StopArea* stopArea(uint8_t index) const {
            if (index < _model._stopAreaNum) {
                return &(_model._stopAreas.at(index));
            }
            else {
                return nullptr;
            }
        }

        inline const std::vector<StopArea>* stopAreas() const {
            return &(_model._stopAreas);
        }

        inline uint32_t safeAreaNum() const {
            return uint32_t(_model._safeAreaNum);
        }

        inline const SafeArea* safeArea(uint8_t index) const {
            if (index < _model._safeAreaNum) {
                return &(_model._safeAreas.at(index));
            }
            else {
                return nullptr;
            }
        }

        inline const std::vector<SafeArea>* safeAreas() const {
            return &(_model._safeAreas);
        }

        inline uint32_t laserNum() const {
            return uint32_t(_model._laserNum);
        }

        inline uint32_t ultrasonicNum() const {
            return uint32_t(_model._ultrasonicNum);
        }

        inline uint32_t DINum() const {
            return uint32_t(_model._DINum);
        }

        inline uint32_t magneticSensorNum() const {
            return uint32_t(_model._magneticSensorNum);
        }

        inline const Laser* laser(uint8_t index) const {
            if (index < _model._laserNum) {
                return &(_model._lasers.at(index));
            }
            else {
                return nullptr;
            }
        }

        inline const Ultrasonic* ultrasonic(uint8_t index) const {
            if (index < _model._ultrasonicNum) {
                return &(_model._ultrasonics.at(index));
            }
            else {
                return nullptr;
            }
        }

        inline const std::vector<Laser>* lasers() const {
            return &(_model._lasers);
        }

        inline const std::vector<Ultrasonic>* ultrasonics() const {
            return &(_model._ultrasonics);
        }

        inline const std::vector<DI>* DIs() const {
            return &(_model._DIs);
        }

        inline const std::vector<DO>* DOs() const {
            return &(_model._DOs);
        }

        inline const std::vector<bool>* DODefaultValues() const {
            return &(_model._DODefaultValues);
        }

        inline const DI* digitalInput(uint8_t index) const {
            if (index < _model._DINum) {
                return &(_model._DIs.at(index));
            }
            else {
                return nullptr;
            }
        }

        inline const RFID* rfid() const {
            return &(_model._RFID);
        }

        inline const std::vector<MagneticSensor>* magneticSensors() const {
            return &(_model._magneticSensors);
        }

        inline const MagneticSensor* magneticSensor(uint8_t index) const {
            if (index < _model._magneticSensorNum) {
                return &(_model._magneticSensors.at(index));
            }
            else {
                return nullptr;
            }
        }

        inline const Vision* vision() const {
            return &(_model._vision);
        }

        const rapidjson::Document* getJson() const {
            return &(_model._modelDoc);
        }

    private:

        class ModelInternal {
        public:

            ModelInternal();

            ~ModelInternal();

            void init(const rapidjson::Document& modelDoc);

            void calibration(const rapidjson::Document& calibDoc);

            void initDecelerateArea();

            void initStopArea();

            void initSafeArea();

            void initChassis();

            void initLaser();

            void initUltrasonic();

            void initDI();

            void initDO();

            void initMagneticSensor();

            void initRFID();

            void initCamera();

            void initSpeaker();
        
        private:
            ModelInternal(const ModelInternal&);

            ModelInternal& operator=(const ModelInternal&);

        public:
            std::string _name;
            std::string _version;

            // 基本参数
            double _width;
            double _head;
            double _tail;
            Mode _mode;
            float _wheelbase;
            std::vector<float> _wheelRadius;
            float _reductionRatio;
            uint32_t _encoderLine;
            uint32_t _maxMotorRpm;
            bool _inverse;
            DriverType _driver;
            bool _useDecelerateArea;
            bool _useStopArea;
            bool _useSafeArea;

            // 简单设备
            bool _gyroExists;
            bool _autoGyroCal;
            bool _brakeExists;
            bool _autoBrake;
            BatteryInfoType _batteryInfoExists;
            bool _LEDExists;

            // 直接设备
            RFIDType _RFIDExists;
            MagneticSensorType _magneticSensorExists;
            bool _laserExists;
            CameraType _cameraExists;
            bool _speakerExists;

            // 接口设备
            uint32_t _maxDINum;
            uint32_t _maxDONum;
            UltrasonicType _ultrasonicExists;
            uint32_t _maxUltrasonicNum;

            std::vector<DecelerateArea> _decelerateAreas;
            uint8_t _decelerateAreaNum;

            std::vector<StopArea> _stopAreas;
            uint8_t _stopAreaNum;

            std::vector<SafeArea> _safeAreas;
            uint8_t _safeAreaNum;

            std::vector<Laser> _lasers;
            uint8_t _laserNum;

            std::vector<Ultrasonic> _ultrasonics;
            uint8_t _ultrasonicNum;

            std::vector<DI> _DIs;
            std::vector<DO> _DOs;
            std::vector<bool> _DODefaultValues;
            uint8_t _DINum;
            uint8_t _DONum;

            RFID _RFID;

            std::vector<MagneticSensor> _magneticSensors;
            uint8_t _magneticSensorNum;

            Vision _vision;

            Speaker _speaker;

            rapidjson::Document _modelDoc;
            rapidjson::Document _calibDoc;
        };

        ModelInternal _model;

        boost::atomic<bool> _init;
    };
} // namespace rbk

#endif // ~_RBK_MODEL_H_
