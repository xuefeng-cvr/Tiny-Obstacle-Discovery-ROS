
(cl:in-package :asdf)

(defsystem "tod-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :std_msgs-msg
)
  :components ((:file "_package")
    (:file "CoData" :depends-on ("_package_CoData"))
    (:file "_package_CoData" :depends-on ("_package"))
    (:file "IMG" :depends-on ("_package_IMG"))
    (:file "_package_IMG" :depends-on ("_package"))
  ))