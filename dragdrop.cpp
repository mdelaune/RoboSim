#include "dragdrop.h"

template<typename T>
T* findParent(QObject* obj)
{
    while (obj) {
        T* result = qobject_cast<T*>(obj);
        if (result)
            return result;
        obj = obj->parent();
    }

    return nullptr;
}

DragDoor::DragDoor(QGraphicsLineItem *doorLine,
                   QGraphicsLineItem *entryLine,
                   QGraphicsScene *scene,
                   QObject *parent)
    : QObject(parent), m_doorLine(doorLine), m_entryLine(entryLine)
{
    m_normalDoorPen = QPen(Qt::black, 4);
    m_selectedDoorPen = QPen(Qt::blue, 4);

    m_normalEntryPen = QPen(Qt::white, 2);
    m_selectedEntryPen = QPen(Qt::cyan, 2);

    m_doorLine->setPen(m_normalDoorPen);
    m_entryLine->setPen(m_normalEntryPen);

    m_doorLine->setCursor(Qt::OpenHandCursor);
    m_entryLine->setCursor(Qt::OpenHandCursor);

    m_doorLine->setAcceptHoverEvents(true);
    m_entryLine->setAcceptHoverEvents(true);

    addToGroup(m_doorLine);
    addToGroup(m_entryLine);

    setFlags(ItemIsSelectable | ItemIsMovable);
    setZValue(1);

    // Automatically update style when selection changes
    connect(scene, &QGraphicsScene::selectionChanged, this, &DragDoor::updateSelectionStyle);
}

void DragDoor::updateSelectionStyle()
{
    if (this->isSelected())
    {
        m_doorLine->setPen(m_selectedDoorPen);
        m_entryLine->setPen(m_selectedEntryPen);
    }
    else
    {
        m_doorLine->setPen(m_normalDoorPen);
        m_entryLine->setPen(m_normalEntryPen);
    }
}

QPointF DragDoor::getOrigin() const
{
    // The origin is at the scene position of the group
    return scenePos();
}

void DragDoor::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::OpenHandCursor);



    QGraphicsItemGroup::mouseReleaseEvent(event);
}

DragObstruction::DragObstruction(const QRectF &body, QRectF *legs, House *house, Obstruction *obstruction)
    : m_body(body)
{
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    setCursor(QCursor(Qt::OpenHandCursor));
    isChest = false;

    for(int i = 0; i < 4; i++)
    {
        m_legs.append(legs[i]);
    }
}

DragObstruction::DragObstruction(const QRectF &body, const QRectF &overlay, House *house, Obstruction *obstruction)
    : m_body(body), m_overlay(overlay)
{
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    setCursor(QCursor(Qt::OpenHandCursor));
    isChest = true;
    m_house = house;
    m_obstruction = obstruction;
}

QRectF DragObstruction::boundingRect() const
{
    QRectF bounds = m_body;

    if(!isChest)
    {
        for(int i = 0; i < m_legs.size(); i++)
        {
            bounds = bounds.united(m_legs[i]);
        }
    }
    else
    {
        bounds = bounds.united(m_overlay);
    }
    //return bounds.adjusted(-5, -5, 5, 5); // margin for selection handles
    return bounds;
}

void DragObstruction::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Draw main body
    painter->setPen(QPen(Qt::black, 2));
    painter->drawRect(m_body);

    if(isChest)
    {
        // Draw overlay
        painter->setBrush(QColor(255, 0, 0, 127));
        painter->setPen(Qt::NoPen);
        painter->drawRect(m_overlay);
    }
    else
    {
        // Draw legs
        painter->setBrush(QColor(255, 0, 0, 127));
        painter->setPen(Qt::NoPen);
        for(const QRectF &leg : m_legs)
        {
            painter->drawEllipse(leg);
        }
    }
}

void DragObstruction::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::ClosedHandCursor);
    QGraphicsItem::mousePressEvent(event);
}

void DragObstruction::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // Handle mouse move events for the obstruction
    QGraphicsItem::mouseMoveEvent(event);
}

void DragObstruction::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::OpenHandCursor);

    QRectF localRect = m_body;
    QRectF sceneRect = mapToScene(localRect).boundingRect();

    if (m_house) {
        m_obstruction->set_topLeft(sceneRect.topLeft());
        m_obstruction->set_bottomRight(sceneRect.bottomRight());
    }

    QGraphicsItem::mouseReleaseEvent(event);
}

DragRoom::DragRoom(const QRectF &rect, QGraphicsScene *scene, House *house, Room *room, long id)
    : QGraphicsRectItem(rect)
{
    setupDraggable(this);
    setBrush(QColor(200, 200, 255, 100));
    setPen(QPen(Qt::black, 2));
    m_id = id;
    m_house = house;
    m_room = room;
}

void DragRoom::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsRectItem::paint(painter, option, widget);

    if (isSelected()) {
        painter->setBrush(Qt::black);
        for (int i = 0; i < 4; ++i) {
            painter->drawRect(handleRect(static_cast<HandlePosition>(i)));
        }
    }
}

DragRoom::HandlePosition DragRoom::handleAt(const QPointF &pos) const
{
    for (int i = 0; i < 4; ++i) {
        if (handleRect(static_cast<HandlePosition>(i)).contains(pos))
            return static_cast<HandlePosition>(i);
    }
    return None;
}

QRectF DragRoom::handleRect(HandlePosition pos) const
{
    QRectF rect = this->rect();
    QPointF offset(-m_handleSize/2, -m_handleSize/2);
    QSizeF size(m_handleSize, m_handleSize);

    switch (pos) {
    case TopLeft:     return QRectF(rect.topLeft() + offset, size);
    case TopRight:    return QRectF(rect.topRight() + offset + QPointF(-m_handleSize, 0), size);
    case BottomLeft:  return QRectF(rect.bottomLeft() + offset + QPointF(0, -m_handleSize), size);
    case BottomRight: return QRectF(rect.bottomRight() + offset + QPointF(-m_handleSize, -m_handleSize), size);
    default:          return QRectF();
    }
}

void DragRoom::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_currentHandle = handleAt(event->pos());
    m_lastMousePos = event->pos();
    m_originalRect = rect();

    if (m_currentHandle != None)
        setCursor(Qt::SizeAllCursor);
    else
        handleMousePress(event, this);

    QGraphicsRectItem::mousePressEvent(event);
}

void DragRoom::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_currentHandle != None) {
        QPointF delta = event->pos() - m_lastMousePos;
        QRectF r = m_originalRect;

        switch (m_currentHandle) {
        case TopLeft:     r.setTopLeft(r.topLeft() + delta); break;
        case TopRight:    r.setTopRight(r.topRight() + delta); break;
        case BottomLeft:  r.setBottomLeft(r.bottomLeft() + delta); break;
        case BottomRight: r.setBottomRight(r.bottomRight() + delta); break;
        default: break;
        }

        prepareGeometryChange();
        setRect(r.normalized());
        update();
    } else {
        QGraphicsRectItem::mouseMoveEvent(event);
    }
}

void DragRoom::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (m_currentHandle != None)
        setCursor(Qt::OpenHandCursor);

    QRectF rect = this->rect(); // in item (local) coordinates
    QPolygonF scenePolygon = this->mapToScene(rect); // maps each corner
    QRectF sceneRect = scenePolygon.boundingRect();

    // Find and update the corresponding Room in the House
    if (m_house) {
        m_room->set_topLeft(sceneRect.topLeft());
        m_room->set_bottomRight(sceneRect.bottomRight());
    }

    m_currentHandle = None;
    QGraphicsRectItem::mouseReleaseEvent(event);
}
