; Auto-generated. Do not edit!


(cl:in-package tod-srv)


;//! \htmlinclude ReqImage-request.msg.html

(cl:defclass <ReqImage-request> (roslisp-msg-protocol:ros-message)
  ((req
    :reader req
    :initarg :req
    :type cl:boolean
    :initform cl:nil))
)

(cl:defclass ReqImage-request (<ReqImage-request>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <ReqImage-request>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'ReqImage-request)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name tod-srv:<ReqImage-request> is deprecated: use tod-srv:ReqImage-request instead.")))

(cl:ensure-generic-function 'req-val :lambda-list '(m))
(cl:defmethod req-val ((m <ReqImage-request>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader tod-srv:req-val is deprecated.  Use tod-srv:req instead.")
  (req m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <ReqImage-request>) ostream)
  "Serializes a message object of type '<ReqImage-request>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'req) 1 0)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <ReqImage-request>) istream)
  "Deserializes a message object of type '<ReqImage-request>"
    (cl:setf (cl:slot-value msg 'req) (cl:not (cl:zerop (cl:read-byte istream))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<ReqImage-request>)))
  "Returns string type for a service object of type '<ReqImage-request>"
  "tod/ReqImageRequest")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'ReqImage-request)))
  "Returns string type for a service object of type 'ReqImage-request"
  "tod/ReqImageRequest")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<ReqImage-request>)))
  "Returns md5sum for a message object of type '<ReqImage-request>"
  "6abf24ca3eea6d1d556c12b7504c3b47")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'ReqImage-request)))
  "Returns md5sum for a message object of type 'ReqImage-request"
  "6abf24ca3eea6d1d556c12b7504c3b47")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<ReqImage-request>)))
  "Returns full string definition for message of type '<ReqImage-request>"
  (cl:format cl:nil "bool req~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'ReqImage-request)))
  "Returns full string definition for message of type 'ReqImage-request"
  (cl:format cl:nil "bool req~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <ReqImage-request>))
  (cl:+ 0
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <ReqImage-request>))
  "Converts a ROS message object to a list"
  (cl:list 'ReqImage-request
    (cl:cons ':req (req msg))
))
;//! \htmlinclude ReqImage-response.msg.html

(cl:defclass <ReqImage-response> (roslisp-msg-protocol:ros-message)
  ((ok
    :reader ok
    :initarg :ok
    :type cl:boolean
    :initform cl:nil))
)

(cl:defclass ReqImage-response (<ReqImage-response>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <ReqImage-response>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'ReqImage-response)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name tod-srv:<ReqImage-response> is deprecated: use tod-srv:ReqImage-response instead.")))

(cl:ensure-generic-function 'ok-val :lambda-list '(m))
(cl:defmethod ok-val ((m <ReqImage-response>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader tod-srv:ok-val is deprecated.  Use tod-srv:ok instead.")
  (ok m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <ReqImage-response>) ostream)
  "Serializes a message object of type '<ReqImage-response>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'ok) 1 0)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <ReqImage-response>) istream)
  "Deserializes a message object of type '<ReqImage-response>"
    (cl:setf (cl:slot-value msg 'ok) (cl:not (cl:zerop (cl:read-byte istream))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<ReqImage-response>)))
  "Returns string type for a service object of type '<ReqImage-response>"
  "tod/ReqImageResponse")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'ReqImage-response)))
  "Returns string type for a service object of type 'ReqImage-response"
  "tod/ReqImageResponse")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<ReqImage-response>)))
  "Returns md5sum for a message object of type '<ReqImage-response>"
  "6abf24ca3eea6d1d556c12b7504c3b47")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'ReqImage-response)))
  "Returns md5sum for a message object of type 'ReqImage-response"
  "6abf24ca3eea6d1d556c12b7504c3b47")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<ReqImage-response>)))
  "Returns full string definition for message of type '<ReqImage-response>"
  (cl:format cl:nil "bool ok~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'ReqImage-response)))
  "Returns full string definition for message of type 'ReqImage-response"
  (cl:format cl:nil "bool ok~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <ReqImage-response>))
  (cl:+ 0
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <ReqImage-response>))
  "Converts a ROS message object to a list"
  (cl:list 'ReqImage-response
    (cl:cons ':ok (ok msg))
))
(cl:defmethod roslisp-msg-protocol:service-request-type ((msg (cl:eql 'ReqImage)))
  'ReqImage-request)
(cl:defmethod roslisp-msg-protocol:service-response-type ((msg (cl:eql 'ReqImage)))
  'ReqImage-response)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'ReqImage)))
  "Returns string type for a service object of type '<ReqImage>"
  "tod/ReqImage")