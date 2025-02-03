#!/bin/bash
cd `dirname $0`
lupdate -recursive .. -ts ../translations/dtkwidget2_*.ts
