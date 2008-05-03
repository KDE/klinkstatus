#!/usr/bin/env kross

CurrentPath = self.action().currentPath()

# $LOAD_PATH << File.dirname( __FILE__  )
$LOAD_PATH << CurrentPath

begin
    require 'korundum4'
rescue LoadError
    error = 'Korundum 4 (KDE 4 bindings for ruby) from kdebindings is required for this script.'
    `kdialog --sorry '#{error}'`
    exit
end

require 'Kross'

begin
    require 'KLinkStatus'
rescue LoadError
    puts "Running script not embedded in KLinkStatus"
    KLinkStatus = Kross::module("KLinkStatus")
end


class Statistics < Qt::Widget

    def initialize(parent = nil)
        super()

        file = Qt::File.new(CurrentPath + "/statisticsui.ui")
        file.open(Qt::File::ReadOnly)
        @widget = Qt::UiLoader.new.load(file, self)
        file.close()

        layout = Qt::VBoxLayout.new
        layout.addWidget(@widget)
        setLayout(layout)

        @totalCheckedSpin = @widget.findChild(Qt::Object, "totalCheckedSpin")
        @goodProgressBar = @widget.findChild(Qt::Object, "goodProgressBar")
        @brokenProgressBar = @widget.findChild(Qt::Object, "brokenProgressBar")
        @undeterminedProgressBar = @widget.findChild(Qt::Object, "undeterminedProgressBar")

        initStatistics()
    end

    def percent(value)
        total = @searchManager.numberOfCheckedLinks()
        return (value / total) * 100
    end

    def initStatistics()
    
        @view = KLinkStatus.view()
        if(@view == nil)
            puts "view is nil"
            return
        end

        @searchManager = @view.activeSearchManager()
        if(@searchManager == nil)
            puts "searchManager is nil"
            return
        end

        @totalCheckedSpin.setValue(@searchManager.numberOfCheckedLinks())
        @goodProgressBar.setValue(percent(@searchManager.numberOfGoodLinks()))
        @brokenProgressBar.setValue(percent(@searchManager.numberOfBrokenLinks()))
        @undeterminedProgressBar.setValue(percent(@searchManager.numberOfUndeterminedLinks()))
    end
end


dialog = Qt::Dialog.new
statsWidget = Statistics.new(dialog)

layout = Qt::VBoxLayout.new()
layout.addWidget(statsWidget)
dialog.setLayout(layout)

dialog.show()
