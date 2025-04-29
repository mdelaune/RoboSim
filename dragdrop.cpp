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
                   House *house,
                   Door *door,
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

    m_house = house;
    m_door = door;
    m_scene = scene;

    addToGroup(m_doorLine);
    addToGroup(m_entryLine);

    setFlags(ItemIsSelectable | ItemIsMovable);
    setZValue(1);

    // Automatically update style when selection changes
    connect(scene, &QGraphicsScene::selectionChanged, this, &DragDoor::updateSelectionStyle);
}

DragDoor::~DragDoor()
{
    if (m_scene) {
        disconnect(m_scene, &QGraphicsScene::selectionChanged, this, &DragDoor::updateSelectionStyle);
    }
}

void DragDoor::updateSelectionStyle()
{
    if (!m_doorLine || !m_entryLine)
        return;

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

    QPointF newOrigin = scenePos();

    if (m_house && m_door) {
        // Update the Door's origin with the new position
        m_door->set_origin(newOrigin);
        qDebug() << "New Origin: " << newOrigin;
    }

    QGraphicsItemGroup::mouseReleaseEvent(event);
}

// Modified DragObstruction class with resizing functionality
DragObstruction::DragObstruction(const QRectF &body, QRectF *legs, House *house, Obstruction *obstruction)
    : m_body(body), m_originalBody(body), m_handleSize(8)  // Increased handle size
{
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    setCursor(QCursor(Qt::OpenHandCursor));
    isChest = false;
    legs_array = legs;
    m_house = house;
    m_obstruction = obstruction;
    m_type = obstruction->get_type();
    m_currentHandle = None;

    // Store the original legs positions and sizes
    for(int i = 0; i < 4; i++)
    {
        m_legs.append(legs_array[i]);
        m_originalLegs.append(legs_array[i]);
    }

    // Set size constraints based on obstruction type
    setSizeConstraints();
}

DragObstruction::DragObstruction(const QRectF &body, const QRectF &overlay, House *house, Obstruction *obstruction)
    : m_body(body), m_originalBody(body), m_overlay(overlay), m_originalOverlay(overlay), m_handleSize(8)  // Increased handle size
{
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    setCursor(QCursor(Qt::OpenHandCursor));
    isChest = true;
    m_house = house;
    m_obstruction = obstruction;
    m_type = obstruction->get_type();
    m_currentHandle = None;

    // Set size constraints based on obstruction type
    setSizeConstraints();
}

void DragObstruction::setSizeConstraints()
{
    // Set minimum and maximum sizes based on obstruction type
    if(m_type == "chest")
    {
        m_minWidth = 40.0;
        m_minHeight = 40.0;
        m_maxWidth = 150.0;
        m_maxHeight = 150.0;
    }
    else if(m_type == "table")
    {
        m_minWidth = 40.0;
        m_minHeight = 40.0;
        m_maxWidth = 150.0;
        m_maxHeight = 150.0;
    }
    else if(m_type == "chair")
    {
        m_minWidth = 30.0;
        m_minHeight = 30.0;
        m_maxWidth = 60.0;
        m_maxHeight = 60.0;
    }


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

    return bounds.adjusted(-m_handleSize, -m_handleSize, m_handleSize, m_handleSize); // margin for selection handles
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

    // Draw resize handles when selected
    if (isSelected()) {
        painter->setPen(QPen(Qt::black, 1));
        painter->setBrush(QColor(0, 0, 0, 180)); // Semi-transparent black

        for (int i = 0; i < 4; ++i) {
            QRectF handleRect = this->handleRect(static_cast<HandlePosition>(i));
            painter->drawRect(handleRect);
        }
    }
}

DragObstruction::HandlePosition DragObstruction::handleAt(const QPointF &pos) const
{
    // Use a larger hit area for detecting handle clicks
    const qreal handleHitPadding = 4.0;

    for (int i = 0; i < 4; ++i) {
        HandlePosition handlePos = static_cast<HandlePosition>(i);
        QRectF handle = handleRect(handlePos);

        // Create an expanded hit area for better handle detection
        QRectF hitArea = handle.adjusted(-handleHitPadding, -handleHitPadding,
                                         handleHitPadding, handleHitPadding);

        if (hitArea.contains(pos))
            return handlePos;
    }
    return None;
}

QRectF DragObstruction::handleRect(HandlePosition pos) const
{
    QRectF rect = m_body;
    QPointF offset(-m_handleSize/2, -m_handleSize/2);
    QSizeF size(m_handleSize, m_handleSize);

    // Create slightly larger hit areas for the handles
    const qreal handlePadding = 2.0;
    QSizeF hitSize(m_handleSize + handlePadding, m_handleSize + handlePadding);
    QPointF hitOffset(-hitSize.width()/2, -hitSize.height()/2);

    switch (pos) {
    case TopLeft:
        // For visual display, we use a slightly larger handle
        return QRectF(rect.topLeft() + offset, size);
    case TopRight:
        return QRectF(rect.topRight() + offset + QPointF(-m_handleSize, 0), size);
    case BottomLeft:
        return QRectF(rect.bottomLeft() + offset + QPointF(0, -m_handleSize), size);
    case BottomRight:
        return QRectF(rect.bottomRight() + offset + QPointF(-m_handleSize, -m_handleSize), size);
    default:
        return QRectF();
    }
}

void DragObstruction::updateLegsPositions(const QRectF &newBody)
{
    if (isChest) {
        // Calculate the scale factor compared to original
        qreal scaleX = newBody.width() / m_originalBody.width();
        qreal scaleY = newBody.height() / m_originalBody.height();

        // Calculate the relative position of the overlay in the original body
        qreal relLeft = (m_originalOverlay.left() - m_originalBody.left()) / m_originalBody.width();
        qreal relTop = (m_originalOverlay.top() - m_originalBody.top()) / m_originalBody.height();
        qreal relWidth = m_originalOverlay.width() / m_originalBody.width();
        qreal relHeight = m_originalOverlay.height() / m_originalBody.height();

        // Apply these relative positions to the new body
        QRectF newOverlay(
            newBody.left() + relLeft * newBody.width(),
            newBody.top() + relTop * newBody.height(),
            relWidth * newBody.width(),
            relHeight * newBody.height()
            );

        m_overlay = newOverlay;
    } else {
        // Define minimum distance from edges (as a percentage of body size)
        const qreal minEdgeDistancePercent = 0.15; // 15% of body width/height

        // Calculate actual minimum distances in pixels
        qreal minDistanceX = newBody.width() * minEdgeDistancePercent;
        qreal minDistanceY = newBody.height() * minEdgeDistancePercent;

        // Calculate safe area boundaries
        qreal safeLeft = newBody.left() + minDistanceX;
        qreal safeRight = newBody.right() - minDistanceX;
        qreal safeTop = newBody.top() + minDistanceY;
        qreal safeBottom = newBody.bottom() - minDistanceY;

        // Update the legs positions relative to the new body
        for (int i = 0; i < m_legs.size(); i++) {
            // Calculate the relative position of leg in original body
            qreal relX = (m_originalLegs[i].center().x() - m_originalBody.left()) / m_originalBody.width();
            qreal relY = (m_originalLegs[i].center().y() - m_originalBody.top()) / m_originalBody.height();

            // Constrain relative positions to stay within safe boundaries
            relX = qBound(minEdgeDistancePercent, relX, 1.0 - minEdgeDistancePercent);
            relY = qBound(minEdgeDistancePercent, relY, 1.0 - minEdgeDistancePercent);

            // Calculate new leg center position with safe boundaries
            QPointF newCenter(
                newBody.left() + relX * newBody.width(),
                newBody.top() + relY * newBody.height()
                );

            // Additional safety check - clamp to safe area if needed
            newCenter.setX(qBound(safeLeft, newCenter.x(), safeRight));
            newCenter.setY(qBound(safeTop, newCenter.y(), safeBottom));

            // Keep leg size consistent
            qreal legWidth = m_originalLegs[i].width();
            qreal legHeight = m_originalLegs[i].height();

            // Update the leg position while keeping its size
            m_legs[i] = QRectF(
                newCenter.x() - legWidth/2,
                newCenter.y() - legHeight/2,
                legWidth,
                legHeight
                );

            // Update the legs array which might be referenced elsewhere
            legs_array[i] = m_legs[i];
        }
    }
}

void DragObstruction::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_currentHandle = handleAt(event->pos());
    m_lastMousePos = event->pos();

    if (m_currentHandle != None)
        setCursor(Qt::SizeAllCursor);
    else
        setCursor(Qt::ClosedHandCursor);

    QGraphicsItem::mousePressEvent(event);
}

//next make sure obstructions are not intersecting rooms befor save
//rotate rooms
//add entry and door coordinates to door class and to json
//unique floorplan id written and read from json

void DragObstruction::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_currentHandle != None) {
        QPointF delta = event->pos() - m_lastMousePos;

        // Add a movement threshold to reduce reactivity
        const qreal movementThreshold = 2.0; // in pixels
        if (qAbs(delta.x()) < movementThreshold && qAbs(delta.y()) < movementThreshold) {
            return; // Ignore tiny movements
        }

        // Store last position for the next move event
        m_lastMousePos = event->pos();

        QRectF newBody = m_body;

        switch (m_currentHandle) {
        case TopLeft:     newBody.setTopLeft(newBody.topLeft() + delta); break;
        case TopRight:    newBody.setTopRight(newBody.topRight() + delta); break;
        case BottomLeft:  newBody.setBottomLeft(newBody.bottomLeft() + delta); break;
        case BottomRight: newBody.setBottomRight(newBody.bottomRight() + delta); break;
        default: break;
        }

        // Normalize rectangle
        newBody = newBody.normalized();

        // Apply size constraints
        if (newBody.width() < m_minWidth) {
            if (m_currentHandle == TopLeft || m_currentHandle == BottomLeft)
                newBody.setLeft(newBody.right() - m_minWidth);
            else
                newBody.setRight(newBody.left() + m_minWidth);
        }
        if (newBody.height() < m_minHeight) {
            if (m_currentHandle == TopLeft || m_currentHandle == TopRight)
                newBody.setTop(newBody.bottom() - m_minHeight);
            else
                newBody.setBottom(newBody.top() + m_minHeight);
        }

        // Apply maximum size constraints
        if (newBody.width() > m_maxWidth) {
            if (m_currentHandle == TopLeft || m_currentHandle == BottomLeft)
                newBody.setLeft(newBody.right() - m_maxWidth);
            else
                newBody.setRight(newBody.left() + m_maxWidth);
        }
        if (newBody.height() > m_maxHeight) {
            if (m_currentHandle == TopLeft || m_currentHandle == TopRight)
                newBody.setTop(newBody.bottom() - m_maxHeight);
            else
                newBody.setBottom(newBody.top() + m_maxHeight);
        }

    } else {
        // Let parent handle the standard move
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void DragObstruction::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::OpenHandCursor);

    // Update the obstruction's position and size in the data model
    QRectF sceneRect = mapToScene(m_body).boundingRect();

    if(m_house && m_obstruction)
    {
        m_obstruction->set_topLeft(sceneRect.topLeft());
        m_obstruction->set_bottomRight(sceneRect.bottomRight());

        if(isChest)
        {
            // Ensure the get_floorCoverage is called to update the overlay
            m_obstruction->get_floorCoverage();
        }
    }

    m_currentHandle = None;
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
    m_scene = scene;
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

bool DragRoom::checkRoomIntersection(const QRectF &newRect)
{
    if (!m_scene)
        return false;

    QRectF sceneRect = mapToScene(newRect).boundingRect();

    // Get all items in the scene
    QList<QGraphicsItem*> allItems = m_scene->items();

    for (QGraphicsItem *item : allItems) {
        // Skip checking against itself
        if (item == this)
            continue;

        // Check if the item is a DragRoom
        DragRoom *otherRoom = dynamic_cast<DragRoom*>(item);
        if (otherRoom) {
            QRectF otherRect = otherRoom->mapRectToScene(otherRoom->rect());

            // Check if rooms intersect with a small tolerance
            // The small margin prevents rooms from being exactly adjacent
            const qreal tolerance = 1.0;
            QRectF adjustedRect = sceneRect.adjusted(tolerance, tolerance, -tolerance, -tolerance);

            if (adjustedRect.intersects(otherRect)) {
                return true; // Intersection detected
            }
        }
    }

    return false; // No intersection
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

        const qreal MIN_WIDTH = 50.0;
        const qreal MIN_HEIGHT = 50.0;

        if (r.width() < MIN_WIDTH) {
            if (m_currentHandle == TopLeft || m_currentHandle == BottomLeft)
                r.setLeft(r.right() - MIN_WIDTH);
            else
                r.setRight(r.left() + MIN_WIDTH);
        }
        if (r.height() < MIN_HEIGHT) {
            if (m_currentHandle == TopLeft || m_currentHandle == TopRight)
                r.setTop(r.bottom() - MIN_HEIGHT);
            else
                r.setBottom(r.top() + MIN_HEIGHT);
        }

        QRectF normalizedRect = r.normalized();
        if (!checkRoomIntersection(normalizedRect)) {
            prepareGeometryChange();
            setRect(normalizedRect);
            update();
        }

    } else {
        QPointF oldPos = pos();

        // Let parent handle the standard move
        QGraphicsRectItem::mouseMoveEvent(event);

        // Check if the move resulted in an intersection
        if (checkRoomIntersection(rect())) {
            // If intersecting, revert to the old position
            setPos(oldPos);
        }
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
