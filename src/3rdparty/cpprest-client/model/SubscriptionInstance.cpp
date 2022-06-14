/**
 * RIC subscription
 * This is the initial REST API for RIC subscription
 *
 * OpenAPI spec version: 0.0.4
 * 
 *
 * NOTE: This class is auto generated by the swagger code generator 2.4.27-SNAPSHOT.
 * https://github.com/swagger-api/swagger-codegen.git
 * Do not edit the class manually.
 */



#include "SubscriptionInstance.h"

namespace io {
namespace swagger {
namespace client {
namespace model {

SubscriptionInstance::SubscriptionInstance()
{
    m_XappEventInstanceId = 0;
    m_E2EventInstanceId = 0;
    m_ErrorCause = utility::conversions::to_string_t("");
    m_ErrorCauseIsSet = false;
    m_ErrorSource = utility::conversions::to_string_t("");
    m_ErrorSourceIsSet = false;
    m_TimeoutType = utility::conversions::to_string_t("");
    m_TimeoutTypeIsSet = false;
}

SubscriptionInstance::~SubscriptionInstance()
{
}

void SubscriptionInstance::validate()
{
    // TODO: implement validation
    const std::string validErrorSource[] = {"SUBMGR", "RTMGR", "DBAAS", "ASN1" \
        "E2Node"};
    const std::string validTimeoutType[] = {"E2-Timeout", "RTMGR-Timeout", "DBAAS-Timeout"};

    if(m_XappEventInstanceId < 0 || m_XappEventInstanceId > 65535)
        INVALID_ARGUMENT(XappEventInstanceId);

    if(m_E2EventInstanceId < 0 || m_E2EventInstanceId > 65535)
        INVALID_ARGUMENT(E2EventInstanceId);

    if(m_ErrorSource.empty())
        INVALID_ARGUMENT(ErrorSource);
    else
    {
        if(std::find(std::begin(validErrorSource), std::end(validErrorSource), m_ErrorSource) \
            == std::end(validErrorSource))
            INVALID_ARGUMENT(ErrorSource);
    }

    if(m_TimeoutType.empty())
        INVALID_ARGUMENT(TimeoutType);
    else 
    {
        if(std::find(std::begin(validTimeoutType), std::end(validTimeoutType), m_TimeoutType) \
            == std::end(validTimeoutType))
            INVALID_ARGUMENT(TimeoutType);
    }

    return;
}

web::json::value SubscriptionInstance::toJson() const
{
    web::json::value val = web::json::value::object();

    val[utility::conversions::to_string_t("XappEventInstanceId")] = ModelBase::toJson(m_XappEventInstanceId);
    val[utility::conversions::to_string_t("E2EventInstanceId")] = ModelBase::toJson(m_E2EventInstanceId);
    if(m_ErrorCauseIsSet)
    {
        val[utility::conversions::to_string_t("ErrorCause")] = ModelBase::toJson(m_ErrorCause);
    }
    if(m_ErrorSourceIsSet)
    {
        val[utility::conversions::to_string_t("ErrorSource")] = ModelBase::toJson(m_ErrorSource);
    }
    if(m_TimeoutTypeIsSet)
    {
        val[utility::conversions::to_string_t("TimeoutType")] = ModelBase::toJson(m_TimeoutType);
    }

    return val;
}

void SubscriptionInstance::fromJson(web::json::value& val)
{
    setXappEventInstanceId(ModelBase::int32_tFromJson(val[utility::conversions::to_string_t("XappEventInstanceId")]));
    setE2EventInstanceId(ModelBase::int32_tFromJson(val[utility::conversions::to_string_t("E2EventInstanceId")]));
    if(val.has_field(utility::conversions::to_string_t("ErrorCause")))
    {
        web::json::value& fieldValue = val[utility::conversions::to_string_t("ErrorCause")];
        if(!fieldValue.is_null())
        {
            setErrorCause(ModelBase::stringFromJson(fieldValue));
        }
    }
    if(val.has_field(utility::conversions::to_string_t("ErrorSource")))
    {
        web::json::value& fieldValue = val[utility::conversions::to_string_t("ErrorSource")];
        if(!fieldValue.is_null())
        {
            setErrorSource(ModelBase::stringFromJson(fieldValue));
        }
    }
    if(val.has_field(utility::conversions::to_string_t("TimeoutType")))
    {
        web::json::value& fieldValue = val[utility::conversions::to_string_t("TimeoutType")];
        if(!fieldValue.is_null())
        {
            setTimeoutType(ModelBase::stringFromJson(fieldValue));
        }
    }
}

void SubscriptionInstance::toMultipart(std::shared_ptr<MultipartFormData> multipart, const utility::string_t& prefix) const
{
    utility::string_t namePrefix = prefix;
    if(namePrefix.size() > 0 && namePrefix.substr(namePrefix.size() - 1) != utility::conversions::to_string_t("."))
    {
        namePrefix += utility::conversions::to_string_t(".");
    }

    multipart->add(ModelBase::toHttpContent(namePrefix + utility::conversions::to_string_t("XappEventInstanceId"), m_XappEventInstanceId));
    multipart->add(ModelBase::toHttpContent(namePrefix + utility::conversions::to_string_t("E2EventInstanceId"), m_E2EventInstanceId));
    if(m_ErrorCauseIsSet)
    {
        multipart->add(ModelBase::toHttpContent(namePrefix + utility::conversions::to_string_t("ErrorCause"), m_ErrorCause));
        
    }
    if(m_ErrorSourceIsSet)
    {
        multipart->add(ModelBase::toHttpContent(namePrefix + utility::conversions::to_string_t("ErrorSource"), m_ErrorSource));
        
    }
    if(m_TimeoutTypeIsSet)
    {
        multipart->add(ModelBase::toHttpContent(namePrefix + utility::conversions::to_string_t("TimeoutType"), m_TimeoutType));
        
    }
}

void SubscriptionInstance::fromMultiPart(std::shared_ptr<MultipartFormData> multipart, const utility::string_t& prefix)
{
    utility::string_t namePrefix = prefix;
    if(namePrefix.size() > 0 && namePrefix.substr(namePrefix.size() - 1) != utility::conversions::to_string_t("."))
    {
        namePrefix += utility::conversions::to_string_t(".");
    }

    setXappEventInstanceId(ModelBase::int32_tFromHttpContent(multipart->getContent(utility::conversions::to_string_t("XappEventInstanceId"))));
    setE2EventInstanceId(ModelBase::int32_tFromHttpContent(multipart->getContent(utility::conversions::to_string_t("E2EventInstanceId"))));
    if(multipart->hasContent(utility::conversions::to_string_t("ErrorCause")))
    {
        setErrorCause(ModelBase::stringFromHttpContent(multipart->getContent(utility::conversions::to_string_t("ErrorCause"))));
    }
    if(multipart->hasContent(utility::conversions::to_string_t("ErrorSource")))
    {
        setErrorSource(ModelBase::stringFromHttpContent(multipart->getContent(utility::conversions::to_string_t("ErrorSource"))));
    }
    if(multipart->hasContent(utility::conversions::to_string_t("TimeoutType")))
    {
        setTimeoutType(ModelBase::stringFromHttpContent(multipart->getContent(utility::conversions::to_string_t("TimeoutType"))));
    }
}

int32_t SubscriptionInstance::getXappEventInstanceId() const
{
    return m_XappEventInstanceId;
}


SubscriptionInstance& SubscriptionInstance::setXappEventInstanceId(int32_t value)
{
    m_XappEventInstanceId = value;
    return *this;
}

int32_t SubscriptionInstance::getE2EventInstanceId() const
{
    return m_E2EventInstanceId;
}


SubscriptionInstance& SubscriptionInstance::setE2EventInstanceId(int32_t value)
{
    m_E2EventInstanceId = value;
    return *this;
}

utility::string_t SubscriptionInstance::getErrorCause() const
{
    return m_ErrorCause;
}


SubscriptionInstance& SubscriptionInstance::setErrorCause(utility::string_t value)
{
    m_ErrorCause = value;
    m_ErrorCauseIsSet = true;
    return *this;
}
bool SubscriptionInstance::errorCauseIsSet() const
{
    return m_ErrorCauseIsSet;
}

SubscriptionInstance& SubscriptionInstance::unsetErrorCause()
{
    m_ErrorCauseIsSet = false;
    return *this;
}

utility::string_t SubscriptionInstance::getErrorSource() const
{
    return m_ErrorSource;
}


SubscriptionInstance& SubscriptionInstance::setErrorSource(utility::string_t value)
{
    m_ErrorSource = value;
    m_ErrorSourceIsSet = true;
    return *this;
}
bool SubscriptionInstance::errorSourceIsSet() const
{
    return m_ErrorSourceIsSet;
}

SubscriptionInstance& SubscriptionInstance::unsetErrorSource()
{
    m_ErrorSourceIsSet = false;
    return *this;
}

utility::string_t SubscriptionInstance::getTimeoutType() const
{
    return m_TimeoutType;
}


SubscriptionInstance& SubscriptionInstance::setTimeoutType(utility::string_t value)
{
    m_TimeoutType = value;
    m_TimeoutTypeIsSet = true;
    return *this;
}
bool SubscriptionInstance::timeoutTypeIsSet() const
{
    return m_TimeoutTypeIsSet;
}

SubscriptionInstance& SubscriptionInstance::unsetTimeoutType()
{
    m_TimeoutTypeIsSet = false;
    return *this;
}

}
}
}
}

