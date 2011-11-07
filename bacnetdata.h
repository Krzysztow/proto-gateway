#ifndef BACNETDATA_H
#define BACNETDATA_H

#include <QtCore>
#include "bacnetcommon.h"
#include "datavisitor.h"

class BacnetTagParser;
namespace Bacnet
{
    class DataVisitor;

    class BacnetDataInterface
    {
    public:
        BacnetDataInterface();
        virtual ~BacnetDataInterface();

        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength) = 0;
        virtual qint32 toRaw(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber) = 0;
        /*Gets BacnetTagParser and tries to parse data so that all its fields are filled. Then returns
        number of bytes used. It'd doesn't remember the tag number it was passed, however may make some
        checks - if application data, make sure we are the one called correctly.*/
        virtual qint32 fromRaw(BacnetTagParser &parser) = 0;
        virtual qint32 fromRaw(BacnetTagParser &parser, quint8 tagNum) = 0;

        virtual bool setInternal(QVariant &value) = 0;
        virtual QVariant toInternal() = 0;

        virtual DataType::DataType typeId() = 0;

    public://visitee interface
        DECLARE_VISITABLE_FUNCTION(BacnetDataInterface)

    protected:
        //! this function encodes the opening (and closing tag) and invokes overridef \sa toRaw(quint8* ptrStart) one.
        qint32 toRawTagEnclosed_helper(quint8 *ptrStart, quint16 buffLength, quint8 tagNumber);
        //! This function decodes the opened tag and invokes overridden \sa fromRaw(BacnetTagParser &parser) one.
        qint32 fromRawTagEnclosed_helper(BacnetTagParser &parser, quint8 tagNum);
        //! Another helper, this time encoding not sequence, but choice value.
        qint32 fromRawChoiceValue_helper(BacnetTagParser &parser, QList<DataType::DataType> choices, Bacnet::BacnetDataInterface *choiceValue);

        template<class T>
        void visitTemplate_helper(T *visitee, DataVisitor *visitor)
        {
            //default implementation
            Q_CHECK_PTR(visitee);
            Q_CHECK_PTR(visitor);
            if (0 != visitor)
                visitor->visit(visitee);
        }
    };
}

#endif // BACNETDATA_H
