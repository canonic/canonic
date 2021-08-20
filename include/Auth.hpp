#pragma once

#include <QObject>
#include <QQmlEngine>


class Auth : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool signedIn READ isUserSignedIn WRITE setUserSignedIn NOTIFY userSignedInChanged)
    Q_PROPERTY(QString userId READ getUserId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString displayName READ getDisplayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(QUrl identityProvider READ getIdentityProvider WRITE setIdentityProvider NOTIFY identityProviderChanged)
    Q_PROPERTY(QString proofOfIdentityType READ getProofOfIdentityType WRITE setProofOfIdentityType NOTIFY proofOfIdentityTypeChanged)
    Q_PROPERTY(QString proofOfIdentity READ getProofOfIdentity WRITE setProofOfIdentity NOTIFY proofOfIdentityChanged)
    Q_PROPERTY(QUrl avatarImageSource READ getAvatarImageSource WRITE setAvatarImageSource NOTIFY avatarImageSourceChanged)

  public:
    bool isUserSignedIn() const;
    void setUserSignedIn(bool signedIn);

    QString getUserId() const;
    void setUserId(QString userId);

    QString getDisplayName() const;
    void setDisplayName(QString displayName);

    QUrl getIdentityProvider() const;
    void setIdentityProvider(QUrl identityProvider);

    QString getProofOfIdentityType() const;
    void setProofOfIdentityType(QString proofOfIdentityType);

    QString getProofOfIdentity() const;
    void setProofOfIdentity(QString proofOfIdentity);

    QUrl getAvatarImageSource() const;
    void setAvatarImageSource(QUrl avatarImgSource);

    Q_INVOKABLE
    void signIn(QString userId,
        QString displayName,
        QUrl identityProvider,
        QString proofOfIdentityType,
        QString proofOfIdentity,
        QUrl avatarImgSource);

    Q_INVOKABLE
    void signOut();

  signals:
    void userSignedInChanged(bool signedIn);
    void userIdChanged(QString userID);
    void displayNameChanged(QString displayName);
    void identityProviderChanged(QUrl identityProvider);
    void proofOfIdentityTypeChanged(QString proofOfIdentityType);
    void proofOfIdentityChanged(QString proofOfIdentity);
    void avatarImageSourceChanged(QUrl avatarImgSource);

  private:
    bool m_signedIn;
    QString m_userId;
    QString m_displayName;
    QUrl m_identityProvider;
    QString m_proofOfIdentityType;
    QString m_proofOfIdentity;
    QUrl m_avatarImageSource;
};
