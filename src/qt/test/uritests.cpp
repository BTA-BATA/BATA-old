// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "uritests.h"

#include "guiutil.h"
#include "walletmodel.h"

#include <QUrl>

void URITests::uriTests()
{
    SendCoinsRecipient rv;
    QUrl uri;
<<<<<<< HEAD
    uri.setUrl(QString("bata:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?req-dontexist="));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("bata:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?dontexist="));
=======
    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?req-dontexist="));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?dontexist="));
>>>>>>> upstream/0.10
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 0);

<<<<<<< HEAD
    uri.setUrl(QString("bata:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?label=Wikipedia Example Address"));
=======
    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?label=Wikipedia Example Address"));
>>>>>>> upstream/0.10
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz"));
    QVERIFY(rv.label == QString("Wikipedia Example Address"));
    QVERIFY(rv.amount == 0);

<<<<<<< HEAD
    uri.setUrl(QString("bata:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?amount=0.001"));
=======
    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?amount=0.001"));
>>>>>>> upstream/0.10
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 100000);

<<<<<<< HEAD
    uri.setUrl(QString("bata:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?amount=1.001"));
=======
    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?amount=1.001"));
>>>>>>> upstream/0.10
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 100100000);

<<<<<<< HEAD
    uri.setUrl(QString("bata:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?amount=100&label=Wikipedia Example"));
=======
    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?amount=100&label=Wikipedia Example"));
>>>>>>> upstream/0.10
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz"));
    QVERIFY(rv.amount == 10000000000LL);
    QVERIFY(rv.label == QString("Wikipedia Example"));

<<<<<<< HEAD
    uri.setUrl(QString("bata:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?message=Wikipedia Example Address"));
=======
    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?message=Wikipedia Example Address"));
>>>>>>> upstream/0.10
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz"));
    QVERIFY(rv.label == QString());

<<<<<<< HEAD
    QVERIFY(GUIUtil::parseBitcoinURI("bata://B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?message=Wikipedia Example Address", &rv));
    QVERIFY(rv.address == QString("B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz"));
    QVERIFY(rv.label == QString());

    uri.setUrl(QString("bata:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?req-message=Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("bata:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?amount=1,000&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("bata:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?amount=1,000.0&label=Wikipedia Example"));
=======
    QVERIFY(GUIUtil::parseBitcoinURI("BATA://B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?message=Wikipedia Example Address", &rv));
    QVERIFY(rv.address == QString("B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz"));
    QVERIFY(rv.label == QString());

    // We currently don't implement the message parameter (ok, yea, we break spec...)
    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?req-message=Wikipedia Example Address"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?amount=1,000&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("BATA:B9BiqLLWuBmHy5P1MGuqyeSj9JNG211DTz?amount=1,000.0&label=Wikipedia Example"));
>>>>>>> upstream/0.10
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));
}
