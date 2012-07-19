

(tool-bar-mode -1)
(menu-bar-mode -1)



(setenv "HOME" "F:/Linux/Programs/emacs")
(setenv "PATH" "F:/Linux/Programs/emacs")
;;set the default file path
(setq default-directory "~/")
(add-to-list 'load-path "~/site-lisp")




;; CC-mode配置
(require 'cc-mode)
(c-set-offset 'inline-open 0)
(c-set-offset 'friend '-)
(c-set-offset 'substatement-open 0)

;;我的C/C++语言编辑策略
(defun my-c-mode-common-hook()
  (setq tab-width 4 indent-tabs-mode nil)
;;hungry-delete and auto-newline
  (c-toggle-auto-hungry-state 1)
;;按键定义
  (define-key c-mode-base-map [(control \`)] 'hs-toggle-hiding)
  (define-key c-mode-base-map [(return)] 'newline-and-indent)
  (define-key c-mode-base-map [(f6)] 'compile)
  (define-key c-mode-base-map [(meta \`)] 'c-indent-command)
  (define-key c-mode-base-map [(tab)] 'my-indent-or-complete)
  (define-key c-mode-base-map [(meta ?/)] 'semantic-ia-complete-symbol-menu)

;;预处理设置
  (setq c-macro-shrink-window-flag t)
  (setq c-macro-preprocessor "cpp")
  (setq c-macro-cppflags " ")
  (setq c-macro-prompt-flag t)
  (setq hs-minor-mode t)
  (setq abbrev-mode t)
)
(add-hook 'c-mode-common-hook 'my-c-mode-common-hook)

;;我的C++语言编辑策略
(defun my-c++-mode-hook()
  (setq tab-width 4 indent-tabs-mode nil)
  (c-set-style "stroustrup")
;;(define-key c++-mode-map [f3] 'replace-regexp)
)





;;—————-cedet setting———————
(load-file "~/cedet/common/cedet.el")
(global-ede-mode 1) ; Enable the Project management system
;; (semantic-load-enable-minimum-features)
;; (semantic-load-enable-code-helpers)
;; (semantic-load-enable-guady-code-helpers)
(semantic-load-enable-excessive-code-helpers)
(semantic-load-enable-semantic-debugging-helpers)
(global-srecode-minor-mode 1) ; Enable template insertion menu


;;配置Semantic的检索范围:
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



;;自定义自动补齐命令，如果在单词中间就补齐，否则就是tab。
(defun my-indent-or-complete ()
  (interactive)
  (if (looking-at "\\>")
    (hippie-expand nil)
    (indent-for-tab-command)
  )
 )
(global-set-key [(control tab)] 'my-indent-or-complete)


;;hippie的自动补齐策略，优先调用了senator的分析结果：
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

;;speedbar关联到了F4上
(global-set-key [(f4)] 'speedbar)


;;emacs默认的compile命令是调用make -k，改成了make。可以把它改成其他的，比如gcc之类的
'(compile-command "make")


;;ecb设置
(add-to-list 'load-path "~/ecb")
(require 'ecb-autoloads)
;;打开emacs，然后M-x ecb-activate即可打开ecb。
(global-set-key [f3] 'ecb-activate)



;;因为gdb-many-windows的窗口太小,有时候不能完全显示一行的内容,所以不得不来回切换gdb-many-windows模式,在绑定快捷键以后就方便多了.
;;start gdb
(global-set-key [f7] 'gdb)
;;toggle gdb-many-windows
(add-hook 'gdb-mode-hook
  '(lambda ()
    (local-set-key [(f8)] 'gdb-many-windows)
  )
)


;;显示等号
(require 'linum)
(setq linum-format "%4d ")
;对所有文件生效
(add-hook 'find-file-hooks (lambda () (linum-mode 1)))


;;加载cscope
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




;;跳到指定行
(define-key   global-map   "\C-c\C-g"   'goto-line)


;;打开显示书签
(enable-visual-studio-bookmarks)
(setq bookmark-save-flag 1)         ;;保存书签
(global-set-key [M-f2] 'bookmark-save)



;;跳转
(global-set-key [f1] 'semantic-ia-fast-jump)
;;回跳

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

;;实现与声明的跳转
(define-key c-mode-base-map [C-S-f1] 'semantic-analyze-proto-impl-toggle)




