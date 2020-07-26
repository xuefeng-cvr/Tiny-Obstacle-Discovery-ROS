; Auto-generated. Do not edit!


(cl:in-package tod-srv)


;//! \htmlinclude StartEval-request.msg.html

(cl:defclass <StartEval-request> (roslisp-msg-protocol:ros-message)
  ((start
    :reader start
    :initarg :start
    :type cl:boolean
    :initform cl:nil))
)

(cl:defclass StartEval-request (<StartEval-request>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <StartEval-request>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'StartEval-request)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name tod-srv:<StartEval-request> is deprecated: use tod-srv:StartEval-request instead.")))

(cl:ensure-generic-function 'start-val :lambda-list '(m))
(cl:defmethod start-val ((m <StartEval-request>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader tod-srv:start-val is deprecated.  Use tod-srv:start instead.")
  (start m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <StartEval-request>) ostream)
  "Serializes a message object of type '<StartEval-request>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'start) 1 0)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <StartEval-request>) istream)
  "Deserializes a message object of type '<StartEval-request>"
    (cl:setf (cl:slot-value msg 'start) (cl:not (cl:zerop (cl:read-byte istream))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<StartEval-request>)))
  "Returns string type for a service object of type '<StartEval-request>"
  "tod/StartEvalRequest")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'StartEval-request)))
  "Returns string type for a service object of type 'StartEval-request"
  "tod/StartEvalRequest")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<StartEval-request>)))
  "Returns md5sum for a message object of type '<StartEval-request>"
  "93d7245f95bf423437148710b6b287c7")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'StartEval-request)))
  "Returns md5sum for a message object of type 'StartEval-request"
  "93d7245f95bf423437148710b6b287c7")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<StartEval-request>)))
  "Returns full string definition for message of type '<StartEval-request>"
  (cl:format cl:nil "bool start~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'StartEval-request)))
  "Returns full string definition for message of type 'StartEval-request"
  (cl:format cl:nil "bool start~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <StartEval-request>))
  (cl:+ 0
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <StartEval-request>))
  "Converts a ROS message object to a list"
  (cl:list 'StartEval-request
    (cl:cons ':start (start msg))
))
;//! \htmlinclude StartEval-response.msg.html

(cl:defclass <StartEval-response> (roslisp-msg-protocol:ros-message)
  ((ok
    :reader ok
    :initarg :ok
    :type cl:boolean
    :initform cl:nil))
)

(cl:defclass StartEval-response (<StartEval-response>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <StartEval-response>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'StartEval-response)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name tod-srv:<StartEval-response> is deprecated: use tod-srv:StartEval-response instead.")))

(cl:ensure-generic-function 'ok-val :lambda-list '(m))
(cl:defmethod ok-val ((m <StartEval-response>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader tod-srv:ok-val is deprecated.  Use tod-srv:ok instead.")
  (ok m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <StartEval-response>) ostream)
  "Serializes a message object of type '<StartEval-response>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'ok) 1 0)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <StartEval-response>) istream)
  "Deserializes a message object of type '<StartEval-response>"
    (cl:setf (cl:slot-value msg 'ok) (cl:not (cl:zerop (cl:read-byte istream))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<StartEval-response>)))
  "Returns string type for a service object of type '<StartEval-response>"
  "tod/StartEvalResponse")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'StartEval-response)))
  "Returns string type for a service object of type 'StartEval-response"
  "tod/StartEvalResponse")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<StartEval-response>)))
  "Returns md5sum for a message object of type '<StartEval-response>"
  "93d7245f95bf423437148710b6b287c7")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'StartEval-response)))
  "Returns md5sum for a message object of type 'StartEval-response"
  "93d7245f95bf423437148710b6b287c7")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<StartEval-response>)))
  "Returns full string definition for message of type '<StartEval-response>"
  (cl:format cl:nil "bool ok~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'StartEval-response)))
  "Returns full string definition for message of type 'StartEval-response"
  (cl:format cl:nil "bool ok~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <StartEval-response>))
  (cl:+ 0
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <StartEval-response>))
  "Converts a ROS message object to a list"
  (cl:list 'StartEval-response
    (cl:cons ':ok (ok msg))
))
(cl:defmethod roslisp-msg-protocol:service-request-type ((msg (cl:eql 'StartEval)))
  'StartEval-request)
(cl:defmethod roslisp-msg-protocol:service-response-type ((msg (cl:eql 'StartEval)))
  'StartEval-response)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'StartEval)))
  "Returns string type for a service object of type '<StartEval>"
  "tod/StartEval")