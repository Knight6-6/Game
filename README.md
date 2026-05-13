# 📑 纸牌游戏程序设计架构文档

## 一、 架构模式：Manager-Centric Design (逻辑中心化设计)

本项目采用高度解耦的结构，将**表现（View）**、逻辑（Logic）**与**流程控制（Glue Code）彻底分离。

### 1. 模块职责划分

| 类名 | 角色 | 核心职责 |
| --- | --- | --- |
| **Card** | **表现/数据对象** | 纯粹的渲染单元。负责：数字/花色展示、翻牌动画（Visual Flip）。不处理任何胜负规则。 |
| **GameManager** | **逻辑控制器** | 游戏的“大脑”。负责：点击判定、**覆盖检测（Overlap Check）**、消除规则判定、**撤销栈管理（Undo Stack）**。 |
| **GameScene** | **胶水/容器类** | 系统的入口。负责：UI 挂载、触摸事件分发、调用 Manager 接口初始化布局、协调 Scene 切换。 |

---

## 二、 核心技术实现：可维护性的保障

### 1. 覆盖判定系统（解决“隔山打牛”）

为了保证代码的可维护性，我们不让 `Card` 互相持有引用，而是由 `GameManager` 统一管理：

* **实现逻辑**：每当牌堆变动，`Manager` 遍历 `_tableau` 容器。
* **判定准则**：若 A 牌与 B 牌矩形相交（`intersectsRect`）且 `B.ZOrder > A.ZOrder`，则 A 被锁定。
* **优势**：未来无论将牌摆成什么形状，这套逻辑都能自动适配，无需修改 `Card` 代码。

### 2. 深度撤销机制（备忘录模式）

通过 `std::stack<Memento>` 记录操作快照。

* **快照内容**：不仅仅记录 `Card*` 指针，更重要的是记录 **`fromPosition`** 和 **`previousZOrder`**。
* **可维护点**：撤销时，Manager 强制将牌还原到原始 ZOrder，确保层级逻辑不会因为多次撤销而混乱。

---

## 三、 加分项：未来需求扩展指南

### 1. 如何新增一种特殊卡牌？（如：炸弹牌、万能牌）

基于当前架构，你不需要重写逻辑，只需执行以下两步：

* **数据层 (Card.h)**: 在 `Card` 类中增加一个 `Type` 枚举或 `isSpecial` 属性。
* **逻辑层 (GameManager.cpp)**: 在 `onCardClicked` 的判定处增加一个插件式的判断：
```cpp
// 扩展非常简单，不影响普通牌逻辑
if (clickedCard->getType() == CardType::BOMB) 
{
    this->executeBombEffect(clickedCard); // 处理爆炸逻辑
} 
else 
{
    this->checkStandardMatch(clickedCard); // 处理标准点数逻辑
}

```

### 2. 如何新增一种回退功能？（如：撤销一整次洗牌）

如果未来客户要求“撤销洗牌”这种大动作，当前的 Memento 结构支持轻松扩展：

* **扩展方案**: 在 `Memento` 结构体中增加一个 `ActionType`。
* **实现逻辑**:
```cpp
void GameManager::undo() 
{
    auto lastAction = _history.top();
    if (lastAction.type == ActionType::SHUFFLE) 
    {
        // 逻辑扩展：遍历所有卡牌，根据记录的坐标集体回位
        this->restoreBoardPositions(lastAction.boardSnapshot);
    }
    else 
    {
        // 原有的单牌回位逻辑
    }
}

```

---

## 四、 结论与交付优势

* **高内聚**: 所有的规则逻辑全在 `GameManager`，修改规则不影响 UI 表现。
* **低耦合**: `Card` 不知道自己在哪，也不知道被谁压着，它只听命于 `Manager` 的 `setLocked` 接口。
* **易扩展**: 通过 `ActionType` 和 `CardType` 的参数化设计，该架构可以承接从“简单连连看”到“复杂卡牌对战”的所有衍生需求。

---