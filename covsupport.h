//#ifndef COVSUPPORT_H
//#define COVSUPPORT_H

//#include <QVariant>
//#include "bacnetprimitivedata.h"

//namespace Bacnet {


//    /////////////////////////////////////////////////////
//    ///////////////CovSupport interface//////////////////
//    /////////////////////////////////////////////////////
//    class CovSupport
//    {
//    public:
//        virtual bool hasChangeOccured(QVariant &actualValue) = 0;
//        virtual void setCovIncrement(BacnetDataInterface *newIncrement) = 0;
//        virtual BacnetDataInterface *covIncrement() = 0;
//    };

//    /////////////////////////////////////////////////////
//    ///////////////////ConcreteCovSupport//////////////////
//    /////////////////////////////////////////////////////
//    template <typename T, class T2>
//            class ConcreteCovSupport:
//            public CovSupport
//    {
//    public:
//        ConcreteCovSupport(T covIncrement, T lastValue = 0);
//        void setCovIncrement(T covIncrement);

//    public://overriden from CovSupport
//        virtual bool hasChangeOccured(QVariant &actualValue);
//        virtual void setCovIncrement(BacnetDataInterface *newIncrement);
//        virtual BacnetDataInterface *covIncrement();

//    private:
//        T _lastValue;
//        T2 _covIncrement;
//    };

//    template <typename T>
//            T toT(QVariant v, bool *ok)
//    {
//        Q_ASSERT_X(false, "covsupport.h", "Not implemented concretization");
//        return T();
//    };

//    //specialized functions
//    template <> inline
//            float toT<float>(QVariant v, bool *ok) {return v.toFloat(ok);}

//    template <> inline
//            double toT<double>(QVariant v, bool *ok) {return v.toDouble(ok);}


//    typedef ConcreteCovSupport<float, Bacnet::Real> RealCovSupport;
//    typedef ConcreteCovSupport<double, Bacnet::Double> DoubleCovSupport;

//    class AnyCovSupport:
//            public CovSupport
//    {
//        public:
//            virtual bool hasChangeOccured(QVariant &actualValue) = 0;
//            virtual void setCovIncrement(BacnetDataInterface *newIncrement) = 0;
//            virtual BacnetDataInterface *covIncrement() = 0;
//    };

//    //    /////////////////////////////////////////////////////
//    //    ///////////////////RealCovSupport////////////////////
//    //    /////////////////////////////////////////////////////
//    //    class RealCovSupport:
//    //            public CovSupport
//    //    {
//    //    public:
//    //        RealCovSupport(float covIncrement, float lastValue = 0);
//    //        void setCovIncrement(float covIncrement);

//    //    public://overriden from CovSupport
//    //        virtual bool hasChangeOccured(QVariant &actualValue);
//    //        virtual void setCovIncrement(BacnetDataInterface *newIncrement);
//    //        virtual BacnetDataInterface *covIncrement();

//    //    private:
//    //        float _lastValue;
//    //        Bacnet::Real _covIncrement;
//    //    };


//    //    /////////////////////////////////////////////////////
//    //    ///////////////////DoubleCovSupport//////////////////
//    //    /////////////////////////////////////////////////////
//    //    class DoubleCovSupport:
//    //            public CovSupport
//    //    {
//    //    public:
//    //        DoubleCovSupport(double covIncrement, double lastValue = 0);
//    //        void setCovIncrement(double covIncrement);

//    //    public://overriden from CovSupport
//    //        virtual bool hasChangeOccured(QVariant &actualValue);
//    //        virtual void setCovIncrement(BacnetDataInterface *newIncrement);
//    //        virtual BacnetDataInterface *covIncrement();

//    //    private:
//    //        double _lastValue;
//    //        Bacnet::Double _covIncrement;
//    //    };
//}
//#endif // COVSUPPORT_H
