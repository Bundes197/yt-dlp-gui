#include <QTest>
#include "maingui.h"

class AppTests : public QObject {
    Q_OBJECT

private slots:
    void testUrlValidation() {
        QVERIFY(MainGUI::isValidUrl("https://www.youtube.com/watch?v=dQw4w9WgXcQ") == true);
        QVERIFY(MainGUI::isValidUrl("http://vimeo.com/12345") == true);

        QVERIFY(MainGUI::isValidUrl("non_sense_url") == false);
        QVERIFY(MainGUI::isValidUrl("") == false);
        QVERIFY(MainGUI::isValidUrl("../../../../") == false);
    }

    void testFilenameSanitization() {
        QCOMPARE(MainGUI::sanitizeFilename("try/traversal../..mp4"), QString("trytraversal.mp4"));
        QCOMPARE(MainGUI::sanitizeFilename("dangerous*symbol?.mp4"), QString("dangeroussymbol.mp4"));
        QCOMPARE(MainGUI::sanitizeFilename("   padding at the start and end   "), QString("padding_at_the_start_and_end"));
        QCOMPARE(MainGUI::sanitizeFilename(".make visible.mp4"), QString("make_visible.mp4"));
        QCOMPARE(MainGUI::sanitizeFilename(".....very/\\ hard?* **test:<>.mp4"), QString("very_hard_test.mp4"));
    }

};

QTEST_MAIN(AppTests)

#include "tests.moc"