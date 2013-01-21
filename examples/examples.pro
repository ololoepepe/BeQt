TEMPLATE = subdirs

!contains(CONFIG, beqt_no_widgets):!contains(CONFIG, beqt_no_codeeditor):SUBDIRS+=simple-editor
