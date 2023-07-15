

#include <QApplication>
#include <QFileSystemModel>
#include <QFileIconProvider>
#include <QScreen>
#include <QScroller>
#include <QTreeView>
#include <QCommandLineParser>
#include <QCommandLineOption>


int main(int argc, char* argv[])
{
    //Створюється об'єкт додатку QApplication, який управляє виконанням програми
    QApplication app(argc, argv);
    //встановлює фактичну стрічку версії
    //QT_VERSION_STR - Цей макрос розширюється до рядка, який визначає номер версії Qt
    //(наприклад, "4.1.2"). Це версія, з якої програма компілюється.
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    //Створюється об'єкт QCommandLineParser для обробки командного рядка програми.
    QCommandLineParser parser;

    //Встановлююється опис програми, що відображається за допомогою helpText()
    parser.setApplicationDescription("Qt Dir View Example");

    //додати інформацію з довідкою, при цьому довідка згенерована автоматично
    //(виходячи з команд, для яких пишуться обробники)
    parser.addHelpOption();
    //додати -v / --version опцію, яка відображає інформацію про версію додатка
    parser.addVersionOption();

    //Встановлюються опції командного рядка. Цей клас використовується для опису параметрів
    //у командному рядку,
    //він дозволяє визначити один і той же параметр різними способами,
    //може бути декілька псевдонімів
    //додається опція -с не використовувати персональні іконки
    QCommandLineOption option("c", "Set QFileSystemModel::DontUseCustomDirectoryIcons");
    parser.addOption(option);
    // додається опція -w  не додавати спостерігачів файлів до шляхів
    QCommandLineOption dontWatchOption("w", "Set QFileSystemModel::DontWatch");
    parser.addOption(dontWatchOption);
    //Ім'я аргументу "directory" та опис "The directory to start in." з'являться під
    //Arguments: розділ довідки
    parser.addPositionalArgument("directory", "The directory to start in.");
    //Обробляє фактичні аргументи командного рядка, надані користувачем
    parser.process(app);
    //повертає список позиційних аргументів або пустий рядок, якщо аргументів нема
    const QString rootPath = parser.positionalArguments().isEmpty() ? QString() : parser.positionalArguments().first();
    //Оголошує model - модель даних для читання локальної файлової системи
    QFileSystemModel model;
    //Встановлюється іконка за замовчуванням для файлів та папок
    QFileIconProvider iconProvider;
    model.setIconProvider(&iconProvider);
    //model.setRootPath("")встановлює поточну папку як папку, з якої модель почне читати
    model.setRootPath("");

    //перевіряє, чи опції було передано парсеру
    if (parser.isSet(option))
        //для моделі встановлює параметр використовувати піктограми каталогу за замовчуванням
        //завжди використовувати піктограми каталогів за замовчуванням, деякі платформи
        //дозволяють користувачам встановлювати різні піктограми, пошук спеціальної
        //піктограми матиме великий вплив на продуктивність програми
        model.setOption(QFileSystemModel::DontUseCustomDirectoryIcons);

    //перевіряє чи опція "Set QFileSystemModel::DontWatch" встановлена у парсері
    if (parser.isSet(dontWatchOption))
        //не додавати спостерігачів файлів до шляхів. Це знижує накладні витрати при використанні
        //моделі для простих завдань
        model.setOption(QFileSystemModel::DontWatchForChanges);
    //візуалізує файлову систему в структурі дерева
    // Створюється об'єкт QTreeView для відображення деревоподібного представлення файлів та папок
    QTreeView tree;
    tree.setModel(&model);
    //Встановлюється кореневий індекс для відображення в дереві, якщо вказаний кореневий шлях є дійсним
    if (!rootPath.isEmpty()) {
        //cleanPath - повертає шлях, в якому нормалізовані роздільники каталогів, тобто рідні роздільники
        //платформи із видаленням надлишкових роздільників
        const QModelIndex rootIndex = model.index(QDir::cleanPath(rootPath));
        if (rootIndex.isValid())
        {
            //Встановлює кореневий елемент на rootIndex
            tree.setRootIndex(rootIndex);
        }
    }
    //Встановлює параметри макета для анімації
    //Якщо ця властивість true, дерево буде анімувати розширення та згортання гілок.
    //Якщо ця властивість дорівнює false, дерево буде розгортати або згортати гілки негайно без показу анімації
    tree.setAnimated(true);
    //Встановлює параметри відступів
    //Ця властивість містить відступи, виміряні в пікселях елементів для кожного рівня в перегляді дерева.
    //Для елементів верхнього рівня відступ визначає горизонтальну відстань від краю вікна перегляду до
    //елементів у першому стовпці; для дочірніх елементів вказує їхній відступ від батьківських елементів.
    //виклик resetIndentation() відновить поведінку за замовчуванням залежно від стилю
    tree.setIndentation(25);
    //Встановлює параметри сортування
    //замінила в своїй програмі на  true, дає сортувати по назві чи по розміру файлів
    tree.setSortingEnabled(true);
    //Встановлює параметри розміру і розташування дерева файлової системи
    //availableGeometry() для отримання доступної області для програм
    //Ця функція застаріла. Це передбачено, щоб старий вихідний код працював.
    //Ми настійно рекомендуємо не використовувати його в новому коді
    //Повертає доступну геометрію екрана
    const QSize availableSize = tree.screen()->availableGeometry().size();
    tree.resize(availableSize / 2);
    //Встановлює ширину першого стовпчика, як третину від ширини дерева
    tree.setColumnWidth(0, tree.width() / 3);
    //Створює QScrollerдля розпізнавання жестів на сенсорних екранах, тож ви можете гортати пальцем у перегляді дерева.
    QScroller::grabGesture(&tree, QScroller::TouchGesture);
    //Встановлює назву вікна з програмою
    tree.setWindowTitle(QObject::tr("Dir View"));
    //запускає відображення дерева
    tree.show();
    return app.exec();
}
