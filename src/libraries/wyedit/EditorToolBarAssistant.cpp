#include "EditorToolBarAssistant.h"
#include "Editor.h"
#include "EditorToolBar.h"


EditorToolBarAssistant::EditorToolBarAssistant(QObject *parent, int iViewMode, QStringList iDisableToolList) : QObject(parent)
{
  if(parent==NULL)
    criticalError("Call "+QString(__FUNCTION__)+" with NULL of parent.");

  editor=qobject_cast<Editor *>(parent);
  viewMode=iViewMode;

  // Создается панель с кнопками
  toolBar=new EditorToolBar( qobject_cast<QWidget *>(parent)->width() );
  toolBar->setObjectName("editorToolBar");

  toolBar->initDisableToolList(iDisableToolList); // Перед инитом устанавливается список скрываемых инструментов

  // Выясняется перечень кнопок в первой строке на панели инструментов
  QStringList toolsListInLine1=editor->editorConfig->get_tools_line_1().split(",");

  // В мобильном режиме добавляется кнопка back (если ее нет)
  if(viewMode==Editor::WYEDIT_MOBILE_MODE && !toolsListInLine1.contains("back"))
  {
    toolsListInLine1.prepend("separator");
    toolsListInLine1.prepend("back");
  }

  // В мобильном режиме добавляется кнопка find_in_base (если ее нет)
  if(viewMode==Editor::WYEDIT_MOBILE_MODE && !toolsListInLine1.contains("find_in_base"))
  {
    toolsListInLine1.append("separator");
    toolsListInLine1.append("find_in_base");
  }

  // Устанавливается перечень кнопок на панели инструментов
  editorToolBar->initToolsLine1(toolsListInLine1); // Первая строка
  editorToolBar->initToolsLine2( editor->editorConfig->get_tools_line_2().split(",") ); // Вторая строка

  // Инициализация панели инструментов
  editorToolBar->init();

  setupSignals();

  currentFontFamily="";
  currentFontSize=0;
  currentFontColor="#000000";
  buttonsSelectColor=QColor(125,170,240,150); // 92,134,198

  // Устанавливается состояние распахнута или нет панель инструментов
  if(editor->editorConfig->get_expand_tools_lines())
    switchExpandToolsLines(1);
  else
    switchExpandToolsLines(-1);
}


EditorToolBarAssistant::~EditorToolBarAssistant()
{
  delete toolBar;
}


EditorToolBarAssistant::setupSignals()
{
  connect(toolBar->expandToolsLines, SIGNAL(clicked()), this, SLOT(onExpandToolsLinesClicked()));
}


EditorToolBar *EditorToolBarAssistant::getToolBar()
{
  return toolBar;
}


// Метод только меняет значение, показываемое списком шрифтов
void EditorToolBarAssistant::setFontselectOnDisplay(QString fontName)
{
  flagSetFontParametersEnabled=false;

  if(fontName.size()>0)
    editorToolBar->fontSelect->setCurrentIndex(editorToolBar->fontSelect->findText(fontName));
  else
    editorToolBar->fontSelect->setCurrentIndex(editorToolBar->fontSelect->count()-1);

  currentFontFamily=fontName;

  flagSetFontParametersEnabled=true;
}


// Метод только меняет значение, показываемое списком размеров шрифта
void EditorToolBarAssistant::setFontsizeOnDisplay(int n)
{
  flagSetFontParametersEnabled=false;

  editorToolBar->fontSize->setCurrentIndex(editorToolBar->fontSize->findData(n));
  currentFontSize=n;

  flagSetFontParametersEnabled=true;
}


// Слот обновления подсветки кнопок выравнивания текста
// Если параметр activate=false, все кнопки будут выставлены в неактивные
// Если параметр activate=true, будет подсвечена кнопка, соответсвующая текущему форматированию
void EditorToolBarAssistant::onUpdateAlignButtonHiglight(bool activate)
{
  QPalette palActive, palInactive;
  palActive.setColor(QPalette::Normal, QPalette::Button, buttonsSelectColor);
  palActive.setColor(QPalette::Normal, QPalette::Window, buttonsSelectColor);

  editorToolBar->alignLeft->setPalette(palInactive);
  editorToolBar->alignCenter->setPalette(palInactive);
  editorToolBar->alignRight->setPalette(palInactive);
  editorToolBar->alignWidth->setPalette(palInactive);

  if(activate==false)return;

  if(textArea->alignment()==Qt::AlignLeft)         editorToolBar->alignLeft->setPalette(palActive);
  else if(textArea->alignment()==Qt::AlignHCenter) editorToolBar->alignCenter->setPalette(palActive);
  else if(textArea->alignment()==Qt::AlignRight)   editorToolBar->alignRight->setPalette(palActive);
  else if(textArea->alignment()==Qt::AlignJustify) editorToolBar->alignWidth->setPalette(palActive);
}


// Обновление подсветки клавиш начертания текста
void EditorToolBarAssistant::updateOutlineButtonHiglight(void)
{
  QPalette palActive, palInactive;
  palActive.setColor(QPalette::Normal, QPalette::Button, buttonsSelectColor);
  palActive.setColor(QPalette::Normal, QPalette::Window, buttonsSelectColor);

  editorToolBar->bold->setPalette(palInactive);
  editorToolBar->italic->setPalette(palInactive);
  editorToolBar->underline->setPalette(palInactive);

  if(textArea->fontWeight()==QFont::Bold) editorToolBar->bold->setPalette(palActive);
  if(textArea->fontItalic()==true)        editorToolBar->italic->setPalette(palActive);
  if(textArea->fontUnderline()==true)     editorToolBar->underline->setPalette(palActive);
}


void EditorToolBarAssistant::setOutlineButtonHiglight(int button, bool active)
{
  QPalette palActive, palInactive;
  palActive.setColor(QPalette::Normal, QPalette::Button, buttonsSelectColor);
  palActive.setColor(QPalette::Normal, QPalette::Window, buttonsSelectColor);

  if(button==BT_BOLD)
  {
    if(active==false) editorToolBar->bold->setPalette(palInactive);
    else              editorToolBar->bold->setPalette(palActive);
    return;
  }

  if(button==BT_ITALIC)
  {
    if(active==false) editorToolBar->italic->setPalette(palInactive);
    else              editorToolBar->italic->setPalette(palActive);
    return;
  }

  if(button==BT_UNDERLINE)
  {
    if(active==false) editorToolBar->underline->setPalette(palInactive);
    else              editorToolBar->underline->setPalette(palActive);
    return;
  }
}


void EditorToolBarAssistant::updateToActualFormat(void)
{
  // Список должен показывать текущий шрифт позиции, где находится курсор
  if(currentFontFamily!=textArea->fontFamily())
    setFontselectOnDisplay(textArea->fontFamily());

  // Размер
  if(currentFontSize!=(int)textArea->fontPointSize())
    setFontsizeOnDisplay((int)textArea->fontPointSize());

  // Кнопки форматирования начертания
  updateOutlineButtonHiglight();

  // Кнопки выравнивания
  onUpdateAlignButtonHiglight(true);
}


void EditorToolBarAssistant::onExpandToolsLinesClicked(void)
{
  switchExpandToolsLines();
}


// Метод, переключающий состояние видимости полной панели инструментов
// Если вызывается без параметра (по умолчанию 0), метод сам переключает
// Параметр 1 - включить полную видимость
// Параметр -1 - выключить полную видимость
void EditorToolBarAssistant::switchExpandToolsLines(int flag)
{
  bool setFlag=true;

  // Если метод был вызван без параметра
  if(flag==0)
  {
    bool is_expand=editorConfig->get_expand_tools_lines();

    if(is_expand) setFlag=false; // Если панель инструментов распахнута, надо сомкнуть
    else setFlag=true; // Иначе распахнуть
  }
  else
  {
    // Иначе метод вызывался с каким-то параметром
    if(flag==1) setFlag=true;
    if(flag==-1) setFlag=false;
  }


  // Панели распахиваются/смыкаются (кроме первой линии инструментов)
  editorToolBar->toolsLine2->setVisible(setFlag);
  if(viewMode==WYEDIT_DESKTOP_MODE)
    indentSliderAssistant->setVisible(setFlag);

  // Запоминается новое состояние
  editorConfig->set_expand_tools_lines(setFlag);

  // Обновляется геометрия расположения движков на слайд-панели.
  // Это необходимо из-за того, что при появлении/скрытии линейки отступов высота области редактирования меняется,
  // и вертикальная прокрутка при соответствующем размере текста может быть видна или не видна.
  // То есть, возможен вариант, когда вертикальная прокрутка появляется при включении видимости слайд-панели,
  // а ее наличие (ее ширина) влияет на ширину и правый движок слайд-панели
  emit updateIndentSliderGeometry();
}


// Включение/выключение подсветки кнопки отображения символов форматирования
void EditorToolBarAssistant::setShowFormattingButtonHiglight(bool active)
{
  QPalette palActive, palInactive;
  palActive.setColor(QPalette::Normal, QPalette::Button, buttonsSelectColor);
  palActive.setColor(QPalette::Normal, QPalette::Window, buttonsSelectColor);

  if(active)
    toolBar->showFormatting->setPalette(palActive);
  else
    toolBar->showFormatting->setPalette(palInactive);
}


bool EditorToolBarAssistant::isKeyForToolLineUpdate(QKeyEvent *event)
{
  if(event->modifiers().testFlag(Qt::ControlModifier) ||
     event->modifiers().testFlag(Qt::AltModifier) ||
     event->modifiers().testFlag(Qt::MetaModifier) ||
     event->key()==Qt::Key_F1 ||
     event->key()==Qt::Key_F2 ||
     event->key()==Qt::Key_F3 ||
     event->key()==Qt::Key_F4 ||
     event->key()==Qt::Key_F5 ||
     event->key()==Qt::Key_F6 ||
     event->key()==Qt::Key_F7 ||
     event->key()==Qt::Key_F8 ||
     event->key()==Qt::Key_F9 ||
     event->key()==Qt::Key_F10 ||
     event->key()==Qt::Key_F11 ||
     event->key()==Qt::Key_F12)
    return true;
  else
    return false;
}


void EditorToolBarAssistant::switchAttachIconExists(bool isExists)
{
  editorToolBar->switchAttachIconExists(isExists);
}
