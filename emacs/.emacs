

(tool-bar-mode -1)
(menu-bar-mode -1)



(setenv "HOME" "F:/Linux/Programs/emacs")
(setenv "PATH" "F:/Linux/Programs/emacs")
;;set the default file path
(setq default-directory "~/")
(add-to-list 'load-path "~/site-lisp")




;; CC-mode����
(require 'cc-mode)
(c-set-offset 'inline-open 0)
(c-set-offset 'friend '-)
(c-set-offset 'substatement-open 0)

;;�ҵ�C/C++���Ա༭����
(defun my-c-mode-common-hook()
  (setq tab-width 4 indent-tabs-mode nil)
;;hungry-delete and auto-newline
  (c-toggle-auto-hungry-state 1)
;;��������
  (define-key c-mode-base-map [(control \`)] 'hs-toggle-hiding)
  (define-key c-mode-base-map [(return)] 'newline-and-indent)
  (define-key c-mode-base-map [(f6)] 'compile)
  (define-key c-mode-base-map [(meta \`)] 'c-indent-command)
  (define-key c-mode-base-map [(tab)] 'my-indent-or-complete)
  (define-key c-mode-base-map [(meta ?/)] 'semantic-ia-complete-symbol-menu)

;;Ԥ��������
  (setq c-macro-shrink-window-flag t)
  (setq c-macro-preprocessor "cpp")
  (setq c-macro-cppflags " ")
  (setq c-macro-prompt-flag t)
  (setq hs-minor-mode t)
  (setq abbrev-mode t)
)
(add-hook 'c-mode-common-hook 'my-c-mode-common-hook)

;;�ҵ�C++���Ա༭����
(defun my-c++-mode-hook()
  (setq tab-width 4 indent-tabs-mode nil)
  (c-set-style "stroustrup")
;;(define-key c++-mode-map [f3] 'replace-regexp)
)





;;����������-cedet setting��������������
(load-file "~/cedet/common/cedet.el")
(global-ede-mode 1) ; Enable the Project management system
;; (semantic-load-enable-minimum-features)
;; (semantic-load-enable-code-helpers)
;; (semantic-load-enable-guady-code-helpers)
(semantic-load-enable-excessive-code-helpers)
(semantic-load-enable-semantic-debugging-helpers)
(global-srecode-minor-mode 1) ; Enable template insertion menu


;;����Semantic�ļ�����Χ:
;;(setq semanticdb-project-roots 
;;  (list
;;    (expand-file-name "~/")
;;  )
;;)

(defconst cedet-user-include-dirs
  (list ".." "../include" "../inc" "../common" "../public"
        "../.." "../../include" "../../inc" "../../common" "../../public"))
(defconst cedet-win32-include-dirs
  (list "D:/MinGW/include"
        "D:/MinGW/include/ddk"
        "D:/MinGW/include/gdb"
        "D:/MinGW/include/gdiplus"
        "D:/MinGW/include/GL"
        "D:/MinGW/include/libltdl"
        "D:/MinGW/include/sys"
        "D:/MinGW/lib/gcc/mingw32/4.7.0/include"
        ;;"D:/Program Files/Microsoft Visual Studio 9.0/VC/include"
    )
)
(require 'semantic-c nil 'noerror)
(let ((include-dirs cedet-user-include-dirs))
  (when (eq system-type 'windows-nt)
    (setq include-dirs (append include-dirs cedet-win32-include-dirs)))
  (mapc (lambda (dir)
          (semantic-add-system-include dir 'c++-mode)
          (semantic-add-system-include dir 'c-mode))
        include-dirs))



;;�Զ����Զ������������ڵ����м�Ͳ��룬�������tab��
(defun my-indent-or-complete ()
  (interactive)
  (if (looking-at "\\>")
    (hippie-expand nil)
    (indent-for-tab-command)
  )
 )
(global-set-key [(control tab)] 'my-indent-or-complete)


;;hippie���Զ�������ԣ����ȵ�����senator�ķ��������
(autoload 'senator-try-expand-semantic "senator")
(setq hippie-expand-try-functions-list
    '(
        senator-try-expand-semantic
        try-expand-dabbrev
        try-expand-dabbrev-visible
        try-expand-dabbrev-all-buffers
        try-expand-dabbrev-from-kill
        try-expand-list
        try-expand-list-all-buffers
        try-expand-line
        try-expand-line-all-buffers
        try-complete-file-name-partially
        try-complete-file-name
        try-expand-whole-kill
    )
)

;;speedbar��������F4��
(global-set-key [(f4)] 'speedbar)


;;emacsĬ�ϵ�compile�����ǵ���make -k���ĳ���make�����԰����ĳ������ģ�����gcc֮���
'(compile-command "make")


;;ecb����
(add-to-list 'load-path "~/ecb")
(require 'ecb-autoloads)
;;��emacs��Ȼ��M-x ecb-activate���ɴ�ecb��
(global-set-key [f3] 'ecb-activate)



;;��Ϊgdb-many-windows�Ĵ���̫С,��ʱ������ȫ��ʾһ�е�����,���Բ��ò������л�gdb-many-windowsģʽ,�ڰ󶨿�ݼ��Ժ�ͷ������.
;;start gdb
(global-set-key [f7] 'gdb)
;;toggle gdb-many-windows
(add-hook 'gdb-mode-hook
  '(lambda ()
    (local-set-key [(f8)] 'gdb-many-windows)
  )
)


;;��ʾ�Ⱥ�
(require 'linum)
(setq linum-format "%4d ")
;�������ļ���Ч
(add-hook 'find-file-hooks (lambda () (linum-mode 1)))


;;����cscope
(require 'xcscope)





(global-set-key [f10] 'compile) 
(global-set-key [f11] 'next-error)   
;; Make compile command                                                                                                                                                              
(defun make-compile-command () 
   (unless (or (file-exists-p "makefile") 
               (file-exists-p "Makefile")) 
     (set (make-local-variable 'compile-command) 
          (concat "g++ " (file-relative-name buffer-file-name) " -Wall -W -pipe -O2 -s && ./a")))) 
(add-hook 'c++-mode-hook 'make-compile-command) 




;;����ָ����
(define-key   global-map   "\C-c\C-g"   'goto-line)


;;����ʾ��ǩ
(enable-visual-studio-bookmarks)
(setq bookmark-save-flag 1)         ;;������ǩ
(global-set-key [M-f2] 'bookmark-save)



;;��ת
(global-set-key [f1] 'semantic-ia-fast-jump)
;;����

(global-set-key [C-f1]
                (lambda ()
                  (interactive)
                  (if (ring-empty-p (oref semantic-mru-bookmark-ring ring))
                      (error "Semantic Bookmark ring is currently empty"))
                  (let* ((ring (oref semantic-mru-bookmark-ring ring))
                         (alist (semantic-mrub-ring-to-assoc-list ring))
                         (first (cdr (car alist))))
                    (if (semantic-equivalent-tag-p (oref first tag)
                                                   (semantic-current-tag))
                        (setq first (cdr (car (cdr alist)))))
                    (semantic-mrub-switch-tags first))))

;;ʵ������������ת
(define-key c-mode-base-map [C-S-f1] 'semantic-analyze-proto-impl-toggle)




