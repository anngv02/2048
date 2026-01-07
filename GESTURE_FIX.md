# Sửa Lỗi Vuốt Chéo - Gesture Handling

## Vấn Đề

Khi người dùng vuốt chéo, các khối di chuyển theo hướng chéo thay vì chỉ di chuyển theo hướng ngang hoặc dọc như game 2048 chuẩn.

## Nguyên Nhân

TouchGFX có thể phát hiện cả `SWIPE_HORIZONTAL` và `SWIPE_VERTICAL` khi vuốt chéo, hoặc chọn một trong hai nhưng với hướng không đúng. Code cũ xử lý cả hai loại gesture mà không kiểm tra góc của swipe.

## Giải Pháp

Sử dụng thuật toán **"Dominant Axis"** (Trục chiếm ưu thế):
- Lưu thông tin drag (điểm bắt đầu và kết thúc)
- Tính toán deltaX và deltaY
- So sánh `absX` và `absY` để xác định hướng chính
- Chỉ xử lý gesture theo hướng chiếm ưu thế

## Các Thay Đổi

### 1. Header Files - Thêm Biến và Hàm

**Tất cả các màn chơi:**
- MainScreenView.hpp
- Screen3x3View.hpp
- Screen5x5View.hpp
- Screen5x5_superMergingView.hpp
- Screen5x5_letterMergingView.hpp

```cpp
protected:
    // Biến để xử lý drag và gesture
    int16_t dragStartX;
    int16_t dragStartY;
    int16_t dragEndX;
    int16_t dragEndY;
    bool isDragging;
    
    // Ngưỡng tối thiểu để tính là 1 lần vuốt (pixel)
    static const int16_t MIN_SWIPE_DISTANCE = 30;
    
    virtual void handleDragEvent(const DragEvent& evt);
    virtual void handleGestureEvent(const GestureEvent& evt);
```

### 2. Source Files - Implement Hàm

**handleDragEvent():**
```cpp
void MainScreenView::handleDragEvent(const DragEvent& evt)
{
    if (evt.getType() == DragEvent::DRAGGED)
    {
        if (!isDragging)
        {
            // Lưu điểm bắt đầu
            dragStartX = evt.getOldX();
            dragStartY = evt.getOldY();
            isDragging = true;
        }
        // Cập nhật điểm cuối liên tục
        dragEndX = evt.getNewX();
        dragEndY = evt.getNewY();
    }
    else if (evt.getType() == DragEvent::DRAGGED_OUT)
    {
        // Reset khi kéo ra khỏi vùng
        isDragging = false;
    }
    
    MainScreenViewBase::handleDragEvent(evt);
}
```

**handleGestureEvent() - Thuật Toán Mới:**
```cpp
void MainScreenView::handleGestureEvent(const GestureEvent& evt)
{
    // Nếu chưa từng nhận Drag trước đó, bỏ qua
    if (!isDragging) return;

    // Tính delta từ điểm bắt đầu và điểm cuối
    int16_t deltaX = dragEndX - dragStartX;
    int16_t deltaY = dragEndY - dragStartY;
    
    // Tính độ dài vector
    int16_t absX = abs(deltaX);
    int16_t absY = abs(deltaY);

    // 1. Kiểm tra độ dài tối thiểu (Lọc nhiễu rung tay)
    if (absX < MIN_SWIPE_DISTANCE && absY < MIN_SWIPE_DISTANCE) {
        isDragging = false;
        return;
    }

        // Nếu di chuyển ngang nhiều hơn dọc -> Là vuốt Ngang
    if (absX > absY) 
    {
        // Đây là vuốt NGANG
        if (deltaX > 0) moveRight();
        else            moveLeft();
    }
    else 
    {
        // Đây là vuốt DỌC
        // Lưu ý: Hệ tọa độ màn hình Y tăng dần xuống dưới
        if (deltaY > 0) moveDown();
        else            moveUp();
    }

    // 3. Reset trạng thái & Xử lý Game logic
    isDragging = false;

    if (hasGridChanged()) {
        spawnRandomTile();
    }
    
    if (isGameOver()) {
        navigateToGameOverScreen();
    }
}
```

### 3. Constructor - Khởi Tạo Biến

```cpp
MainScreenView::MainScreenView()
{
    // ... gán tiles ...
    
    // Khởi tạo biến drag
    dragStartX = 0;
    dragStartY = 0;
    dragEndX = 0;
    dragEndY = 0;
    isDragging = false;
}
```

### 4. Includes - Thêm DragEvent

```cpp
#include <touchgfx/events/DragEvent.hpp>
#include <cstdlib>  // Cho hàm abs()
```

## Thuật Toán "Dominant Axis"

### Cách Hoạt Động

1. **Lưu thông tin drag**: Khi người dùng bắt đầu kéo, lưu điểm bắt đầu (`dragStartX`, `dragStartY`)
2. **Cập nhật điểm cuối**: Liên tục cập nhật điểm cuối (`dragEndX`, `dragEndY`) trong quá trình drag
3. **Tính toán delta**: Khi nhận gesture event, tính `deltaX = dragEndX - dragStartX` và `deltaY = dragEndY - dragStartY`
4. **So sánh độ lớn**: So sánh `absX` và `absY`
   - Nếu `absX > absY` → Vuốt ngang (xử lý theo deltaX)
   - Nếu `absY > absX` → Vuốt dọc (xử lý theo deltaY)
5. **Lọc nhiễu**: Chỉ xử lý nếu `absX >= MIN_SWIPE_DISTANCE` hoặc `absY >= MIN_SWIPE_DISTANCE`

### Ví Dụ

**Vuốt chéo từ trái-trên sang phải-dưới:**
```
dragStartX = 50, dragStartY = 50
dragEndX = 200, dragEndY = 150

deltaX = 150, deltaY = 100
absX = 150, absY = 100

absX > absY → Vuốt ngang
deltaX > 0 → moveRight() ✓
```

**Vuốt chéo từ trái-dưới sang phải-trên:**
```
dragStartX = 50, dragStartY = 200
dragEndX = 150, dragEndY = 50

deltaX = 100, deltaY = -150
absX = 100, absY = 150

absY > absX → Vuốt dọc
deltaY < 0 → moveUp() ✓
```

## Ưu Điểm

1. **Chính xác**: Chỉ xử lý hướng chiếm ưu thế, bỏ qua vuốt chéo
2. **Hiệu quả**: Dùng `abs()` thay vì `%` - nhanh hơn cho MCU
3. **Lọc nhiễu**: `MIN_SWIPE_DISTANCE` lọc các cử chỉ nhỏ không mong muốn
4. **Đơn giản**: Logic rõ ràng, dễ maintain

## Các Màn Đã Sửa

✅ MainScreenView (4x4)
✅ Screen3x3View (3x3)
✅ Screen5x5View (5x5)
✅ Screen5x5_superMergingView (5x5 Super Merging)
✅ Screen5x5_letterMergingView (5x5 Letter Merging)

## Kết Quả

Sau khi sửa:
- ✅ Vuốt chéo sẽ chỉ di chuyển theo hướng chiếm ưu thế (ngang hoặc dọc)
- ✅ Không còn di chuyển "lạ" khi vuốt chéo
- ✅ Hoạt động giống game 2048 chuẩn

