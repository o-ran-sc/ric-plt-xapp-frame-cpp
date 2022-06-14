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



#include "XAppConfig.h"

namespace io {
namespace swagger {
namespace client {
namespace model {

XAppConfig::XAppConfig()
{
}

XAppConfig::~XAppConfig()
{
}

void XAppConfig::validate()
{
    // TODO: implement validation
    if(m_Metadata.get())
    {
        try {
            m_Metadata->validate();
        }
        catch(const std::invalid_argument& e)
        {
            INVALID_ARGUMENT(Metadata);
        }
    }
    else
        INVALID_ARGUMENT(Metadata);

    return;
}

web::json::value XAppConfig::toJson() const
{
    web::json::value val = web::json::value::object();

    val[utility::conversions::to_string_t("metadata")] = ModelBase::toJson(m_Metadata);

    return val;
}

void XAppConfig::fromJson(web::json::value& val)
{
    std::shared_ptr<ConfigMetadata> newMetadata(new ConfigMetadata());
    newMetadata->fromJson(val[utility::conversions::to_string_t("metadata")]);
    setMetadata( newMetadata );
}

void XAppConfig::toMultipart(std::shared_ptr<MultipartFormData> multipart, const utility::string_t& prefix) const
{
    utility::string_t namePrefix = prefix;
    if(namePrefix.size() > 0 && namePrefix.substr(namePrefix.size() - 1) != utility::conversions::to_string_t("."))
    {
        namePrefix += utility::conversions::to_string_t(".");
    }

    m_Metadata->toMultipart(multipart, utility::conversions::to_string_t("metadata."));
}

void XAppConfig::fromMultiPart(std::shared_ptr<MultipartFormData> multipart, const utility::string_t& prefix)
{
    utility::string_t namePrefix = prefix;
    if(namePrefix.size() > 0 && namePrefix.substr(namePrefix.size() - 1) != utility::conversions::to_string_t("."))
    {
        namePrefix += utility::conversions::to_string_t(".");
    }

    std::shared_ptr<ConfigMetadata> newMetadata(new ConfigMetadata());
    newMetadata->fromMultiPart(multipart, utility::conversions::to_string_t("metadata."));
    setMetadata( newMetadata );
}

std::shared_ptr<ConfigMetadata> XAppConfig::getMetadata() const
{
    return m_Metadata;
}


XAppConfig& XAppConfig::setMetadata(std::shared_ptr<ConfigMetadata> value)
{
    m_Metadata = value;
    return *this;
}
}
}
}
}
