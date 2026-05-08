{
    "groups": [
        {
            "key": "base",
            "name": qsTranslate("FileManagerSettings", "基础"),
            "groups": [
                {
                    "key": "open_action",
                    "name": qsTranslate("FileManagerSettings", "打开行为"),
                    "options": [
                        {
                            "key": "always_open_on_new_window",
                            "text": qsTranslate("FileManagerSettings", "总是在新窗口打开文件夹"),
                            "type": "checkbox",
                            "default": false
                        },
                        {
                            "key": "open_file_action",
                            "name": qsTranslate("FileManagerSettings", "打开文件："),
                            "type": "combobox",
                            "items": [
                                qsTranslate("FileManagerSettings", "单击"),
                                qsTranslate("FileManagerSettings", "双击")
                            ],
                            "default": 1
                        }
                    ]
                },
                {
                    "key": "default_view",
                    "name": qsTranslate("FileManagerSettings", "默认视图"),
                    "options": [
                        {
                            "key": "view_mode",
                            "name": qsTranslate("FileManagerSettings", "视图模式："),
                            "type": "combobox",
                            "items": [
                                qsTranslate("FileManagerSettings", "图标"),
                                qsTranslate("FileManagerSettings", "列表")
                            ],
                            "default": 0
                        },
                        {
                            "key": "show_hidden",
                            "text": qsTranslate("FileManagerSettings", "显示隐藏文件"),
                            "type": "checkbox",
                            "default": false
                        }
                    ]
                }
            ]
        },
        {
            "key": "advance",
            "name": qsTranslate("FileManagerSettings", "高级"),
            "groups": [
                {
                    "key": "preview",
                    "name": qsTranslate("FileManagerSettings", "预览"),
                    "options": [
                        {
                            "key": "text_file_preview",
                            "text": qsTranslate("FileManagerSettings", "文本文件预览"),
                            "type": "checkbox",
                            "default": true
                        },
                        {
                            "key": "image_file_preview",
                            "text": qsTranslate("FileManagerSettings", "图片预览"),
                            "type": "checkbox",
                            "default": true
                        }
                    ]
                },
                {
                    "key": "mount",
                    "name": qsTranslate("FileManagerSettings", "挂载"),
                    "options": [
                        {
                            "key": "auto_mount",
                            "text": qsTranslate("FileManagerSettings", "自动挂载可移动设备"),
                            "type": "checkbox",
                            "default": true
                        },
                        {
                            "key": "auto_mount_and_open",
                            "text": qsTranslate("FileManagerSettings", "自动挂载后打开"),
                            "type": "checkbox",
                            "default": false
                        }
                    ]
                }
            ]
        }
    ]
}
