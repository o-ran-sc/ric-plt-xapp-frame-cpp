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

/*
 * SubscriptionDetail.h
 *
 * 
 */

#ifndef IO_SWAGGER_CLIENT_MODEL_SubscriptionDetail_H_
#define IO_SWAGGER_CLIENT_MODEL_SubscriptionDetail_H_


#include "../ModelBase.h"

#include "ActionToBeSetup.h"
#include <vector>

namespace io {
namespace swagger {
namespace client {
namespace model {

/// <summary>
/// 
/// </summary>
class  SubscriptionDetail
    : public ModelBase
{
public:
    SubscriptionDetail();
    virtual ~SubscriptionDetail();

    /////////////////////////////////////////////
    /// ModelBase overrides

    void validate() override;

    web::json::value toJson() const override;
    void fromJson(web::json::value& json) override;

    void toMultipart(std::shared_ptr<MultipartFormData> multipart, const utility::string_t& namePrefix) const override;
    void fromMultiPart(std::shared_ptr<MultipartFormData> multipart, const utility::string_t& namePrefix) override;

    /////////////////////////////////////////////
    /// SubscriptionDetail members

    /// <summary>
    /// 
    /// </summary>
    int32_t getXappEventInstanceId() const;
        SubscriptionDetail& setXappEventInstanceId(int32_t value);
    /// <summary>
    /// 
    /// </summary>
    std::vector<int32_t>& getEventTriggers();
        SubscriptionDetail& setEventTriggers(std::vector<int32_t> value);
    /// <summary>
    /// 
    /// </summary>
    std::vector<std::shared_ptr<ActionToBeSetup>>& getActionToBeSetupList();
        SubscriptionDetail& setActionToBeSetupList(std::vector<std::shared_ptr<ActionToBeSetup>> value);

protected:
    int32_t m_XappEventInstanceId;
        std::vector<int32_t> m_EventTriggers;
        std::vector<std::shared_ptr<ActionToBeSetup>> m_ActionToBeSetupList;
    };

}
}
}
}

#endif /* IO_SWAGGER_CLIENT_MODEL_SubscriptionDetail_H_ */