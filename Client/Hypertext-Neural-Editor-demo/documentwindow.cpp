/****************************************4
* Команда 2
* Гипертекстовый редактор
*
* Код класса документа в шаблоне MDI
*****************************************/
#include <QtWidgets>
#include <QFileDialog>
#include <QTextDocumentWriter>
#include <QColor>
#include <QColorDialog>

#include "documentwindow.h"
#include "qevent.h"

DocumentWindow::DocumentWindow(QWidget* pParent /* = nullptr */) :
    QTextBrowser(pParent)
{
    setOpenExternalLinks(true);
    setReadOnly(false);
    setTextInteractionFlags(Qt::TextSelectableByMouse |
                            Qt::LinksAccessibleByMouse |
                            Qt::TextSelectableByMouse |
                            Qt::TextSelectableByKeyboard |
                            Qt:: TextEditable);
    setUndoRedoEnabled(true);
}

// Метод загрузки файла и чтения из него текста
bool DocumentWindow::OpenFile(const QString &pathFileName)
{
    if (pathFileName.isEmpty())
    {
        emit SignalStatusBarMessage(tr("Path empty!"));
        return false;
    }
    
    QFile file(pathFileName);

    if(file.open(QIODevice::ReadOnly))
    {
        QGuiApplication::setOverrideCursor(Qt::WaitCursor);

        QByteArray data = file.readAll();
        file.close();
        QMimeDatabase db;
        const QString &mimeTypeName = db.mimeTypeForFileNameAndData(pathFileName, data).name();
        if (mimeTypeName == u"text/html")
        {

            auto encoding = QStringDecoder::encodingForHtml(data);
            QString str = QStringDecoder(encoding ? *encoding : QStringDecoder::Utf8)(data);
            QUrl fileUrl =
                pathFileName.startsWith(u':') ?
                               QUrl(pathFileName) : QUrl::fromLocalFile(pathFileName);
            document()->setBaseUrl(fileUrl.adjusted(QUrl::RemoveFilename));
            setHtml(str);

            QFileInfo fi(pathFileName);
            setSearchPaths({fi.path()});
            setSource(fi.fileName());
        }
        else if (mimeTypeName == u"text/markdown")
            setMarkdown(QString::fromUtf8(data));
        else
            setPlainText(QString::fromUtf8(data));

        QFileInfo fi(pathFileName);
        QString fileName = fi.fileName();
        setWindowTitle(fileName);

        QDir::setCurrent(fi.path());

        _pathFileName = pathFileName;

        emit SignalStatusBarMessage(
            tr("File opened: \"%1\"").arg(QDir::toNativeSeparators(pathFileName)));
        QGuiApplication::restoreOverrideCursor();
        emit IsOpen(pathFileName);
        return true;
    }
    else
    {
        emit SignalStatusBarMessage(
            tr("File could not open: \"%1\"").arg(QDir::toNativeSeparators(pathFileName)));
        return false;
    }
}

// Перегруженный метод закрытия виджета
void DocumentWindow::closeEvent(QCloseEvent *event)
{
    if (isChanged)
    {
        QMessageBox msgBox;
        QPushButton *yesBtn = msgBox.addButton(tr("Yes"), QMessageBox::YesRole);
        msgBox.addButton(tr("No"), QMessageBox::NoRole);
        msgBox.setWindowTitle(tr("Closing"));
        auto name = windowTitle();
        name = name.mid(0, (name.length()-1));
        msgBox.setText(tr("Document ") + name + tr(" not saved! Close?"));

        msgBox.exec();

        if (msgBox.clickedButton() == yesBtn)
        {
            emit IsClose(_pathFileName);
            event->accept();
        }
        else
            event ->ignore();
    }
    else
    {
        emit IsClose(_pathFileName);
        event->accept();
    }
}

// Перегруженный метод нажатия клавиши
void DocumentWindow::keyPressEvent(QKeyEvent *e)
{
    if(!isChanged)
    {
        setWindowTitle(windowTitle() + "*");
        isChanged = true;
    }
    QTextBrowser::keyPressEvent(e);
}

// Метод загрузки документа
QString DocumentWindow::Load()
{
    // Антон us2_t-003 Спринт 1: Реализовать загрузку файла
    QFileDialog fileDialog(nullptr, tr("Open File..."), QDir::currentPath());
    fileDialog.setOptions(QFileDialog::DontUseNativeDialog);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setMimeTypeFilters({"text/html", "text/markdown", "text/plain"});
    if (fileDialog.exec() != QDialog::Accepted)
        return "";

    const QString pathFileName = fileDialog.selectedFiles().constFirst();

    return pathFileName;
}

// Метод сохранения документа
bool DocumentWindow::Save()
{
    // Александр us2_t-002 Спринт 1: Реализовать сохранение файла
    if (_pathFileName.isEmpty())
        return SaveAs();
    else
        return SaveFile(_pathFileName);
}

// Метод сохранить документ как...
bool DocumentWindow::SaveAs()
{
    // Александр us2_t-002 Спринт 1: Реализовать сохранение файла
    QFileDialog fileDialog(this, tr("Save as..."), QDir::currentPath());
    fileDialog.setOptions(QFileDialog::DontUseNativeDialog);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList mimeTypes{"text/html",
                          "text/plain",
                          "application/vnd.oasis.opendocument.text",
                          "text/markdown"};
    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.setDefaultSuffix("html");
    connect(&fileDialog, SIGNAL(filterSelected(QString)),
            SLOT(SlotSetDefaultSuffix()));
    if (fileDialog.exec() != QDialog::Accepted)
        return false;
    const QString pathFileName = fileDialog.selectedFiles().constFirst();

    if (pathFileName.isEmpty())
        return false;

    if (SaveFile(pathFileName))
    {
        QFileInfo fi(pathFileName);
        auto suffix = fi.suffix();
        if (suffix == "html" || suffix == "htm")
        {
            setSearchPaths({fi.path()});
            setSource(fi.fileName());
        }

        _pathFileName = pathFileName;

        return true;
    }
    else
        return false;
}

// Метод сохранение файла
bool DocumentWindow::SaveFile(const QString& pathFileName)
{
    QString errorMessage;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);

    QTextDocumentWriter writer(pathFileName);

    if(!writer.write(document()))
    {
        errorMessage = tr("Cannot save file %1")
                           .arg(QDir::toNativeSeparators(pathFileName));
    }

    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty())
    {
        QMessageBox::warning(this, tr("Save file"), errorMessage);
        return false;
    }

    QFileInfo fi(pathFileName);
    setWindowTitle(fi.fileName());
    isChanged = false;
    return true;
}

// Метод сохранить документ как *.odt
void DocumentWindow::SaveAsOdt(const QString fileName)
{
    QTextDocumentWriter writer;
    writer.setFormat("odf");
    writer.setFileName(fileName);
    writer.write(document());
}

// устанавливает жирный шрифт
void DocumentWindow::TextBold(bool checked)
{
    QTextCharFormat fmt;
    fmt.setFontWeight(checked ? QFont::Bold : QFont::Normal);
    MergeFormatOnWordOrSelection(fmt);
}

// устанавливает подчеркнутый шрифт
void DocumentWindow::TextUnderline(bool checked)
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(checked);
    MergeFormatOnWordOrSelection(fmt);
}

// устанавливает курсивный шрифт
void DocumentWindow::TextItalic(bool checked)
{
    QTextCharFormat fmt;
    fmt.setFontItalic(checked);
    MergeFormatOnWordOrSelection(fmt);
}

// устанавливает шрифт
void DocumentWindow::TextFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamilies({f});
    MergeFormatOnWordOrSelection(fmt);
}

// выбор и установка цвета шрифта
void DocumentWindow::TextColor()
{
    QColor color = QColorDialog::getColor(this->textColor(), this);
    if (!color.isValid())
    {
        return;
    }
    QTextCharFormat format;
    format.setForeground(color);
    MergeFormatOnWordOrSelection(format);
}

// устанавливает формат текста
void DocumentWindow::MergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = this->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    this->mergeCurrentCharFormat(format);
}

// устанавливает размер шрифта
void DocumentWindow::TextSize(const QString &size)
{
    float pointSize = size.toFloat();
    if (pointSize > 0)
    {
        QTextCharFormat format;
        format.setFontPointSize(pointSize);
        this->mergeCurrentCharFormat(format);
    }
}

// Поиск в тексте
void DocumentWindow::Find(QString searchRequest, bool wholeText,
                          bool caseSensitive, bool backward)
{
    bool found = false;

    QTextDocument::FindFlags flags;
    if(wholeText)
        flags |= QTextDocument::FindWholeWords;
    if(caseSensitive)
        flags |= QTextDocument::FindCaseSensitively;
    if(backward)
        flags |= QTextDocument::FindBackward;

    QTextCursor cursor = textCursor();
    cursor.clearSelection();

    cursor = document()->find(searchRequest, cursor, flags);
    if(!cursor.isNull())
    {
        found = true;
        if (backward)
        {
            QTextCursor saveCursor = cursor;
            cursor.setPosition(cursor.position(), QTextCursor::MoveAnchor);
            cursor.setPosition(saveCursor.position()-searchRequest.length(),
                               QTextCursor::KeepAnchor);
            setTextCursor(cursor);
        }
        else
            setTextCursor(cursor);

        activateWindow();
    }

    if (!found)
        QMessageBox::information(this, tr("Find"), tr("Sequence not found!"));
}

// Возвращает выделенный текст
QString DocumentWindow::GetSelectedText()
{
    return textCursor().selectedText();
}

// Возвращает текст документа как plainText для использования в качестве контекста
QString DocumentWindow::GetContext()
{
    return toPlainText();
}

// Создание гипертекстовой ссылки
void DocumentWindow::MakeHyperlink(const QString linkText, QString linkTarget)
{
    QFileInfo fit(linkTarget);
    auto suffix = fit.suffix();

    if (suffix != "html" && suffix != "htm")
        linkTarget += ".html";

    document()->setDefaultStyleSheet(
        QString("body { font-size: %1pt; font-weight: %2; font-family %3; }")
            .arg(textCursor().charFormat().font().pointSize())
            .arg(fontWeight())
            .arg(fontFamily()));

    //Конветируем абсолютные пути в относительные
    QFileInfo fi(_pathFileName);
    int pathPos = linkTarget.indexOf(fi.path());
    if (pathPos >= 0)
    {
        QString relativePath = linkTarget.mid(
            (pathPos + fi.path().length())+1, linkTarget.length());
        if (!relativePath.isEmpty())
            linkTarget = relativePath;
    }

    insertHtml(QString("<body><a href=\"%1\">%2</a></body>").arg(linkTarget, linkText));

    Save();
}

// Добавить изображение
void DocumentWindow::AddImage()
{
    QFileDialog fileDialog(this, tr("Add image"), QDir::currentPath());
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setNameFilter("Images files (*.png *.jpeg *.jpg *.jpe *.xpm)");

    if (fileDialog.exec() != QDialog::Accepted)
        return;

    QString pathFileName = fileDialog.selectedFiles().constFirst();

    if (pathFileName.isEmpty())
        return;

    //Конветируем абсолютные пути в относительные
    QFileInfo fi(_pathFileName);
    int pathPos = pathFileName.indexOf(fi.path());
    QString relativePath;
    if (pathPos >= 0)
        relativePath = pathFileName.mid(
            (pathPos + fi.path().length())+1, pathFileName.length());

    QImage* image = new QImage();
    QFile file(relativePath);
    if(!relativePath.isEmpty() && file.exists())
    {
        if (!image->load(relativePath))
        {
            qDebug() << "Load error";
            image->load(pathFileName);
            relativePath = pathFileName;
        }
    }
    else
    {
        qDebug() << "Make local error";
        image->load(pathFileName);
        relativePath = pathFileName;
    }

    QTextDocument* textDocument = document();
    textDocument->addResource(QTextDocument::ImageResource,
                              relativePath, QVariant(*image));
    QTextCursor cursor = textCursor();
    QTextImageFormat imageFormat;
    imageFormat.setWidth(image->width());
    imageFormat.setHeight(image->height());
    imageFormat.setName(relativePath);
    cursor.insertImage(imageFormat);
}

//метод устанавливает выравнивание текста
void DocumentWindow::TextAlign(Qt::Alignment alignment)
{
    this->setAlignment(alignment);
}

//метод увеличивает отступ на шаг
void DocumentWindow::Indent()
{
    ModifyIndentation(1);
}

//метод уменьшает отступ на шаг
void DocumentWindow::Unindent()
{
    ModifyIndentation(-1);
}

//метод изменяет отсутп в соответствии с заданным значением
//(положительное число -увеличение, отрицательно число - уменьшение)
void DocumentWindow::ModifyIndentation(int amount)
{
    QTextCursor cursor = this->textCursor();
    cursor.beginEditBlock();
    if (cursor.currentList())
    {
        QTextListFormat listFmt = cursor.currentList()->format();
        QTextCursor above(cursor);
        above.movePosition(QTextCursor::Up);
        if (above.currentList() &&
            listFmt.indent() + amount == above.currentList()->format().indent())
            above.currentList()->add(cursor.block());
        else
        {
            listFmt.setIndent(listFmt.indent() + amount);
            cursor.createList(listFmt);
        }
    }
    else
    {
        QTextBlockFormat blockFmt = cursor.blockFormat();
        blockFmt.setIndent(blockFmt.indent() + amount);
        cursor.setBlockFormat(blockFmt);
    }
    cursor.endEditBlock();
}

//метод устанавливает цвета фона
void DocumentWindow::BackgroundColor()
{
    QColor cur_color;
    if (this->currentCharFormat().background().style()!=Qt::BrushStyle::NoBrush)
    {
        cur_color = this->textBackgroundColor();
    }
    else
    {
        cur_color = QColor(255,255,255);
    }
    QColor color = QColorDialog::getColor(cur_color, this);
    if (!color.isValid())
        return;

    this->setTextBackgroundColor(color);
}

// Установить расширение по умолчанию
void DocumentWindow::SlotSetDefaultSuffix()
{
    auto mimeFilter = qobject_cast<QFileDialog*>(sender())->selectedMimeTypeFilter();

    if (mimeFilter == "text/html")
        qobject_cast<QFileDialog*>(sender())->setDefaultSuffix("html");
    if (mimeFilter == "text/plain")
        qobject_cast<QFileDialog*>(sender())->setDefaultSuffix("txt");
    if (mimeFilter == "application/vnd.oasis.opendocument.text")
        qobject_cast<QFileDialog*>(sender())->setDefaultSuffix("odt");
    if (mimeFilter == "text/markdown")
        qobject_cast<QFileDialog*>(sender())->setDefaultSuffix("md");
}

// Устанавливает стиль текста
void DocumentWindow::TextStyle(int styleIndex)
{
    QTextCursor cursor = this->textCursor();
    QTextListFormat::Style style = QTextListFormat::ListStyleUndefined;
    QTextBlockFormat::MarkerType marker = QTextBlockFormat::MarkerType::NoMarker;

    switch (styleIndex) {
    case 1:
        style = QTextListFormat::ListDisc;
        break;
    case 2:
        style = QTextListFormat::ListCircle;
        break;
    case 3:
        style = QTextListFormat::ListSquare;
        break;
    case 4:
        if (cursor.currentList())
            style = cursor.currentList()->format().style();
        else
            style = QTextListFormat::ListDisc;
        marker = QTextBlockFormat::MarkerType::Unchecked;
        break;
    case 5:
        if (cursor.currentList())
            style = cursor.currentList()->format().style();
        else
            style = QTextListFormat::ListDisc;
        marker = QTextBlockFormat::MarkerType::Checked;
        break;
    case 6:
        style = QTextListFormat::ListDecimal;
        break;
    case 7:
        style = QTextListFormat::ListLowerAlpha;
        break;
    case 8:
        style = QTextListFormat::ListUpperAlpha;
        break;
    case 9:
        style = QTextListFormat::ListLowerRoman;
        break;
    case 10:
        style = QTextListFormat::ListUpperRoman;
        break;
    default:
        break;
    }

    cursor.beginEditBlock();

    QTextBlockFormat blockFormat = cursor.blockFormat();

    if (style == QTextListFormat::ListStyleUndefined)
    {
        blockFormat.setObjectIndex(-1);
        int headingLevel = styleIndex >= 11 ? styleIndex - 11 + 1 : 0; // H1 to H6, or Standard
        blockFormat.setHeadingLevel(headingLevel);
        cursor.setBlockFormat(blockFormat);

        int sizeAdjustment = headingLevel ? 4 - headingLevel : 0; // H1 to H6: +3 to -2
        QTextCharFormat fmt;
        fmt.setFontWeight(headingLevel ? QFont::Bold : QFont::Normal);
        fmt.setProperty(QTextFormat::FontSizeAdjustment, sizeAdjustment);
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.mergeCharFormat(fmt);
        this->mergeCurrentCharFormat(fmt);
    }
    else
    {
        blockFormat.setMarker(marker);
        cursor.setBlockFormat(blockFormat);
        QTextListFormat listFmt;
        if (cursor.currentList())
            listFmt = cursor.currentList()->format();
        else
        {
            listFmt.setIndent(blockFormat.indent() + 1);
            blockFormat.setIndent(0);
            cursor.setBlockFormat(blockFormat);
        }
        listFmt.setStyle(style);
        cursor.createList(listFmt);
    }

    cursor.endEditBlock();
}
