
(cl:in-package :asdf)

(defsystem "tod-srv"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "ReqImage" :depends-on ("_package_ReqImage"))
    (:file "_package_ReqImage" :depends-on ("_package"))
    (:file "StartEval" :depends-on ("_package_StartEval"))
    (:file "_package_StartEval" :depends-on ("_package"))
  ))