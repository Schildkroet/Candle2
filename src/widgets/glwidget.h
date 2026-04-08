// This file is a part of "Candle" application.
// Copyright 2015-2016 Hayrullin Denis Ravilevich

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QTime>
#include "drawers/shaderdrawable.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget();
    void addDrawable(ShaderDrawable *drawable);
    void updateExtremes(ShaderDrawable *drawable);
    void fitDrawable(ShaderDrawable *drawable = nullptr);
    bool antialiasing() const;
    void setAntialiasing(bool antialiasing);

    QTime spendTime() const;
    void setSpendTime(const QTime &spendTime);

    QTime estimatedTime() const;
    void setEstimatedTime(const QTime &estimatedTime);

    double lineWidth() const;
    void setLineWidth(double lineWidth);

    void setIsometricView();
    void setTopView();
    void setFrontView();
    void setLeftView();

    int fps();
    void setFps(int fps);

    QString parserStatus() const;
    void setParserStatus(const QString &parserStatus);

    QString bufferState() const;
    void setBufferState(const QString &bufferState);

    bool zBuffer() const;
    void setZBuffer(bool zBuffer);

    bool updatesEnabled() const;
    void setUpdatesEnabled(bool updatesEnabled);

    bool msaa() const;
    void setMsaa(bool msaa);

    QColor colorBackground() const;
    void setColorBackground(const QColor &colorBackground);

    QColor colorText() const;
    void setColorText(const QColor &colorText);

    double pointSize() const;
    void setPointSize(double pointSize);

    bool vsync() const;
    void setVsync(bool vsync);

    QString speedState() const;
    void setSpeedState(const QString &speedState);

    QString pinState() const;
    void setPinState(const QString &pinState);

    QVector3D modelLowerBounds() const { return QVector3D(m_xMin, m_yMin, m_zMin); }
    QVector3D modelUpperBounds() const { return QVector3D(m_xMax, m_yMax, m_zMax); }
    QVector3D modelRanges() const { return QVector3D(m_xSize, m_ySize, m_zSize); }
    int vertexCount() const { return m_vertices; }
    double windowSizeWorld() const { return m_distance; }
    const QMatrix4x4 &projectionMatrix() const { return m_projectionMatrix; }
    const QMatrix4x4 &viewMatrix() const { return m_viewMatrix; }
    const QList<ShaderDrawable*> &shaderDrawables() const { return m_shaderDrawables; }

signals:
    void rotationChanged();
    void resized();

public slots:

private slots:
    void onFramesTimer();
    void viewAnimation();

private:
    double m_xRot, m_yRot, m_xLastRot, m_yLastRot;
    double m_xPan, m_yPan, m_xLastPan, m_yLastPan;
    double m_xLookAt, m_yLookAt, m_zLookAt;
    QPoint m_lastPos;
    double m_zoom;
    double m_distance;
    double m_xMin, m_xMax, m_yMin, m_yMax, m_zMin, m_zMax, m_xSize, m_ySize, m_zSize;
    double m_lineWidth;
    double m_pointSize;
    bool m_antialiasing;
    bool m_msaa;
    bool m_vsync;
    bool m_zBuffer;
    int m_frames = 0;
    int m_fps = 0;
    int m_vertices = 0;
    int m_targetFps;
    int m_animationFrame;
    QTime m_spendTime;
    QTime m_estimatedTime;
    QBasicTimer m_timerPaint;
    double m_xRotTarget, m_yRotTarget;
    double m_xRotStored, m_yRotStored;
    bool m_animateView;
    QString m_parserStatus;
    QString m_speedState;
    QString m_pinState;
    QString m_bufferState;
    bool m_updatesEnabled;

    double normalizeAngle(double angle);
    double calculateVolume(QVector3D size);
    void beginViewAnimation();
    void stopViewAnimation();

    QList<ShaderDrawable*> m_shaderDrawables;
    QOpenGLShaderProgram *m_shaderProgram;
    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_viewMatrix;

    QColor m_colorBackground;
    QColor m_colorText;

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;
    void updateProjection();
    void updateView();

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *we) override;

    void timerEvent(QTimerEvent *) override;
};

#endif // GLWIDGET_H
