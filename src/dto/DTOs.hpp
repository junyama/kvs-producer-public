#ifndef DTOs_hpp
#define DTOs_hpp

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 *  Data Transfer Object. Object containing fields only.
 *  Used in API for serialization/deserialization and validation
 */
class StatusDto : public oatpp::DTO {
  
    DTO_INIT(StatusDto, DTO)

    DTO_FIELD(Int32, statusCode);
    DTO_FIELD(Int32, returnCode);
    
    DTO_FIELD_INFO(message) {
    info->description = "Verbose message";
    }
    DTO_FIELD(String, message);
  
};

class GstreamerConfigObj : public oatpp::DTO {

    DTO_INIT(GstreamerConfigObj, DTO)

    DTO_FIELD(String, device) = "/dev/video0";
    DTO_FIELD(UInt16, width) = 640;
    DTO_FIELD(UInt16, height) = 480;
    DTO_FIELD(UInt8, FPS) = 30;
    DTO_FIELD(Boolean, timeStamp) = true;
    DTO_FIELD(String, encoder) = "omxh264enc";
    DTO_FIELD(String, streamName) = "gst_test";
    DTO_FIELD(UInt16, StorageSize) = 512;
    DTO_FIELD(String, AccessKey) = "AKIA2W5GL7UUMFHV54F4";
    DTO_FIELD(String, secretKey) = "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";
    DTO_FIELD(String, awsRegion) = "ap-south-1";
    
    DTO_FIELD(String, kvsAddMetadataStructureName) = "kvs-add-metadata";
    DTO_FIELD(String, kvsAddMetadataName) = "name";
    DTO_FIELD(String, kvsAddMetadataValue) = "value";
    DTO_FIELD(String, kvsAddMetadataPersistent) = "persist";
    
};

#include OATPP_CODEGEN_END(DTO)

#endif /* DTOs_hpp */
