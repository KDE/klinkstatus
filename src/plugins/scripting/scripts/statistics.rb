$LOAD_PATH << File.dirname( __FILE__  )

begin
    require 'korundum4'
rescue LoadError
    error = 'Korundum 4 (KDE 4 bindings for ruby) from kdebindings is required for this script.'
    `kdialog --sorry '#{error}'`
    exit
end

# require 'Qt'

require 'Kross'
require 'KLinkStatus'
require 'statisticsui.rb'

# KLinkStatus = Kross::module("KLinkStatus")

class Statistics < Qt::Widget

    def initialize(parent = nil)
        super()

        @ui = Ui_StatisticsUi.new
        @ui.setupUi(self)

#         view = KLinkStatus.view()
#         if(view == nil)
#             puts "view is nil"
#             return
#         end
# 
#         searchManager = view.activeSearchManager()
#         if(searchManager == nil)
#             puts "searchManager is nil"
#             return
#         end
# 
#         puts searchManager.numberOfCheckedLinks()

    end
end


dialog = Qt::Dialog.new

statsWidget = Statistics.new(dialog)

# layout = Qt::VBoxLayout.new()
# layout.addWidget(statsWidget)
# 
# dialog.setLayout(layout)

dialog.exec()

