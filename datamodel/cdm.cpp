#include "cdm.h"

#include "property.h"
#include "propertyowner.h"
#include "propertyobserver.h"
#include "propertysubject.h"

DataModel *DataModel::_instance = 0;
const int DataModel::UNUSED_TIME_VALUE = std::numeric_limits<int>::min();

DataModel::DataModel(QObject *parent):
    QObject(parent),
    _internalTimeout_ms(DEFAULT_TIMEOUT),
    _untakenProperties(0)
{
    initiateAsynchIds();

    startTimer(100*_internalTimeout_ms);
}

DataModel::~DataModel()
{
}

DataModel *DataModel::instance()
{
    if (0 == _instance)
        _instance = new DataModel();

    return _instance;
}

PropertySubject *DataModel::createPropertySubject(quint32 propId, QVariant::Type propertyType)
{
    if (_properties.contains(propId)) {
        qDebug("%s : Property subject with id: %d exists. this mapping is not goint to be created", __PRETTY_FUNCTION__, propId);
        Q_ASSERT_X(false, "DataModel::createProperty()", "The property subject may be created only once!");
        return 0;
    }

    PropertySubject *propS(0);
    if (0 != _untakenProperties) { //we have started factory. Maybe, the subject got already created!
        propS = _untakenProperties->take(propId);
        if (0 != propS)
            propS->setType(propertyType);
    }

    if (0 == propS)
        propS = new PropertySubject(0, propertyType);

    _properties.insert(propId, propS);
    return propS;
}

static const char InternalPropertyTypeAttribute[]    = "int-type";
static const char InternalPropertyId[]               = "int-id";
static const char ObserverAttributeValue[]           = "observer";
static const char SubjectAttributeValue[]            = "subject";
static const char InternalVariantType[]              = "int-var-type";

static const char InternalConverterType[]           = "int-conv-type";
static const char InternalConverterNoneValue[]      = "none";
static const char InternalConverterUniversalValue[] = "universal";
static const char InternalConverterScalingValue[]   = "scaling";
static const char InternalConverterScalingFactor[]  = "int-conv-factor"; //when we have scaling converter, it requires a factor to be set.
static const char InternalConverterBitmaskValue[]   = "masking";
static const char InternalConverterBitmaskMask[]    = "int-conv-mask";  //when we use BirmaskConverter, we have to be provided with mask.

#include "configuratorhelper.h"
#include "propertywithconversionobserver.h"
#include "propertyconverter.h"

enum ConverterTypes {
    None,                   //corresponds to non-converting property -> PropertyObserver should be created
    UniversalConverter,     //corresponds to PropertyUniversalConverter
    ScalingConverter,       //corresponds to PropertyScalerConverter
    BitMaskConverter        //correspodns to PropertyBitmaslConverter
};

PropertyObserver *DataModel::createPropertyObserver(QDomElement &observerElement)
{
    bool ok;
    quint32 internalId = observerElement.attribute(InternalPropertyId).toUInt(&ok);

    if (!internalId) {
        ConfiguratorHelper::elementError(observerElement, InternalPropertyId);
        return 0;
    }

    QString type = observerElement.attribute(InternalConverterType);
    //! \todo Maybe some mapping of name to ConverterTypes would be nice

    PropertySubject *observerSubject = propertySubjectForId_helper(internalId);
    if (0 == observerSubject) {
        qDebug("%s : Couldn't get propery Subject for id: %d", __PRETTY_FUNCTION__, internalId);
        return 0;
    }

    if ("" == type || InternalConverterNoneValue == type)
        return new PropertyObserver(0, observerSubject);
    else {
        if (InternalConverterUniversalValue == type) {          //universal conversion is needed
            return new DataModelNS::PropertyWithConversionObserver(0, observerSubject, 0); //zero converter means using UniversalConverter by default. Thanks to that, we spare some memory
        } else if (InternalConverterScalingValue == type) {     //conversion with scaling factor. We need the factor as well - it's obligatory.
            float factor = observerElement.attribute(InternalConverterScalingFactor).toFloat(&ok);
            if (!ok || 0 == factor) {
                ConfiguratorHelper::elementError(observerElement, InternalConverterScalingFactor);
                return 0;
            }
            return new DataModelNS::PropertyWithConversionObserver(0, observerSubject, new DataModelNS::PropertyScalerConverter(factor));
        } else if (InternalConverterBitmaskValue == type) {
            QString mask = observerElement.attribute(InternalConverterBitmaskMask);
            QBitArray bitMask;
            if (!mask.isEmpty())
                bitMask = ConfiguratorHelper::bitArrayFromString(mask, &ok);
            if (!ok || bitMask.isNull()) {
                ConfiguratorHelper::elementError(observerElement, InternalConverterBitmaskMask);
                return 0;
            }
            return new DataModelNS::PropertyWithConversionObserver(0, observerSubject, new DataModelNS::PropertyBitmaskConverter(bitMask));
        }
    }

    return 0;
}

PropertySubject *DataModel::createPropertySubject(QDomElement &subjectElement)
{
    bool ok;
    quint32 internalId = subjectElement.attribute(InternalPropertyId).toUInt(&ok);
    if (!ok)
        return 0;
    QString variantType = subjectElement.attribute(InternalVariantType);
    uint type = QMetaType::type(variantType.toLatin1());
    if (QVariant::Invalid == type)
        return 0;
    return createPropertySubject(internalId, (QVariant::Type)type);
}

Property *DataModel::createProperty(QDomElement &propElement)
{
    if (!propElement.hasAttribute(InternalPropertyTypeAttribute) || !propElement.hasAttribute(InternalPropertyId)) {
        qDebug("%s : Property has no required attributes! %s | %s", __PRETTY_FUNCTION__,
               qPrintable(propElement.attribute(InternalPropertyTypeAttribute)),
               qPrintable(propElement.attribute(InternalPropertyId)));
        return 0;
    }

    QString str = propElement.attribute(InternalPropertyTypeAttribute);
    if (ObserverAttributeValue == str) {
        return createPropertyObserver(propElement);
    } else if (SubjectAttributeValue == str) {
        return createPropertySubject(propElement);
    }

    return 0;
}

void DataModel::startFactory()
{
    if (0 == _untakenProperties)
        _untakenProperties = new QMap<quint32, PropertySubject*>();
}

void DataModel::stopFactory()
{
    if (0 == _untakenProperties) {
        qDebug("%s : method was called without start counterpart or many times!", __PRETTY_FUNCTION__);
    } else {
        if (!_untakenProperties->isEmpty()) {
            qDebug("%s : factory stopped, but there are some property subjects left and not taken!", __PRETTY_FUNCTION__);
            QMap<quint32, PropertySubject*>::Iterator it = _untakenProperties->begin();
            for (; it != _untakenProperties->end(); ++it) {
                qDebug("# Property subject of type %s and id %d is dangling.", QVariant::typeToName((*it)->type()), it.key());
            }
        } else {//all fine
            delete _untakenProperties;
            _untakenProperties = 0;
        }
    }
}

PropertySubject *DataModel::getProperty(quint32 propId)
{
    return _properties.value(propId);
}

PropertySubject *DataModel::propertySubjectForId_helper(quint32 propId)
{
    PropertySubject *propS = _properties.value(propId);
    if ( (0 == propS) && (0 != _untakenProperties) ) {
        //maybe property was already created!
        propS = _untakenProperties->value(propId);
        if (0 == propS) {//it was not, so create it
            propS = new PropertySubject(0, QVariant::Invalid);
            _untakenProperties->insert(propId, propS);
        }
    }

    return propS;
}


PropertyObserver *DataModel::createPropertyObserver(quint32 propId)
{
    PropertySubject *propS = propertySubjectForId_helper(propId);
    if (0 == propS)
        return 0;

    PropertyObserver *propO = new PropertyObserver(0, propS);
    return propO;
}

void DataModel::initiateAsynchIds()
{
    _asynchIdStates.reserve(MAX_ASYNCH_ID);
    AsynchIdEntry nullEntry = {UNUSED_TIME_VALUE, 0, 0};
    for (int i = 0; i < MAX_ASYNCH_ID; ++i) {
        _asynchIdStates.append(nullEntry);
    }
}

int DataModel::generateAsynchId()
{
    bool found = false;
    int id(0);
    for (; id < MAX_ASYNCH_ID; ++id) {
        if (isAsynchIdUnused(id)) {
            found = true;
            break;
        }
    }

    if (!found)
        return -1;

    _asynchIdStates[id].subjectProperty = 0;
    _asynchIdStates[id].requestingObserver = 0;
    _asynchIdStates[id].timeLeft = _internalTimeout_ms;

    ++id;
    return id;
}

void DataModel::setAsynchIdData(int asynchId, PropertySubject *subject, PropertyObserver *requester)
{
    --asynchId;
    Q_ASSERT(!isAsynchIdUnused(asynchId));
    _asynchIdStates[asynchId].subjectProperty = subject;
    _asynchIdStates[asynchId].requestingObserver = requester;
}

PropertySubject *DataModel::asynchActionSubject(int asynchId)
{
    --asynchId;
    Q_ASSERT(!isAsynchIdUnused(asynchId));
    if (isAsynchIdUnused(asynchId))
        return 0;
    else
        return _asynchIdStates[asynchId].subjectProperty;
}

PropertyObserver *DataModel::asynchActionRequester(int asynchId)
{
    --asynchId;
    Q_ASSERT(!isAsynchIdUnused(asynchId));
    if (isAsynchIdUnused(asynchId))
        return 0;
    else
        return _asynchIdStates[asynchId].requestingObserver;
}

void DataModel::releaseAsynchId(int id)
{
    --id;
    Q_ASSERT_X(id <= MAX_ASYNCH_ID, "DataModel::releaseAsynchId()", "Trying to release id, that is out of range!");
    if (id > MAX_ASYNCH_ID)
        return;

    //make sure this is used. However when events like timer timeout and frame received are used, it could happen
    //there will be two consecutive calls, which will make assertion fail.
    Q_ASSERT_X(!isAsynchIdUnused(id), "DataModel::releaseAsynchId()", "Attemp to release id that is already released!");

    setAsynchIdUnused(id);
}

void DataModel::timerEvent(QTimerEvent *)
{
    for (int id = 0; id<MAX_ASYNCH_ID; ++id) {
        if (!isAsynchIdUnused(id)) {
            if (_asynchIdStates[id].timeLeft <= _internalTimeout_ms) {
                if (_asynchIdStates[id].timeLeft < 0) {//the time this transaction is outstanding is within [_internalTimeout_ms, 2*_internalTimeout_ms). Time to clean!
                    if (_asynchIdStates[id].subjectProperty != 0) {
                        _asynchIdStates[id].subjectProperty;//timeout INTERNAL_TIMEOUT
                    }
                    if (_asynchIdStates[id].requestingObserver != 0) {
                        _asynchIdStates[id].requestingObserver->asynchActionFinished(id, Property::InternalTimeout);//timeout INTERNAL_TIMEOUT
                    }
                    setAsynchIdUnused(id);
                } else {//clean it next time
                    _asynchIdStates[id].timeLeft = -1;//mark it to be cleaned next time
                }
            }
        } else {//transaction relatively new
            _asynchIdStates[id].timeLeft -= _internalTimeout_ms;
        }
    }
    qDebug("Cleaning is started!");
}
