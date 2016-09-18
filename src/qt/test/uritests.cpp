#include "uritests.h"
#include "../guiutil.h"
#include "../walletmodel.h"

#include <QUrl>

void URITests::uriTests()
{
    SendCoinsRecipient rv;
    QUrl uri;
    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?req-dontexist="));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?dontexist="));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 0);

    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?label=Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz"));
    QVERIFY(rv.label == QString("Wikipedia Example Address"));
    QVERIFY(rv.amount == 0);

    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?amount=0.001"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 100000);

    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?amount=1.001"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 100100000);

    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?amount=100&label=Wikipedia Example"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz"));
    QVERIFY(rv.amount == 10000000000LL);
    QVERIFY(rv.label == QString("Wikipedia Example"));

    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?message=Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz"));
    QVERIFY(rv.label == QString());

    QVERIFY(GUIUtil::parseBitcoinURI("BATA://B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?message=Wikipedia Example Address", &rv));
    QVERIFY(rv.address == QString("B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz"));
    QVERIFY(rv.label == QString());

    // We currently don't implement the message parameter (ok, yea, we break spec...)
    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?req-message=Wikipedia Example Address"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?amount=1,000&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?amount=1,000.0&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));
}
