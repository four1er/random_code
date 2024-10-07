/**
 * @file observer.cpp
 * @author four1er 92024373@qq.com
 * @brief 观察者模式
 * 情景：高数课，ABCD四位同学，A是好学生，去上课，B在寝室睡觉，C在网吧打游戏，D在学校外陪女友逛街
 * 他们约定，如果要点名了，A在QQ群里吼一声，他们立刻赶到教室去。
 * 采用观察者模式实现这个情景的应用。
 * @version 0.1
 * @date 2024-10-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <iostream>
#include <list>
#include <string>

namespace tf {
class Observer;

class Subject {
 public:
  virtual ~Subject() = default;
  virtual void attach(Observer* observer) = 0;
  virtual void detach(Observer* observer) = 0;
  virtual void notify(const std::string& msg) = 0;
};

class Observer {
 public:
  virtual ~Observer() = default;
  virtual void update(const std::string& msg) = 0;
  virtual std::string getName() = 0;

 protected:
  Observer() {}
};

class QQGroup : public Subject {
 public:
  QQGroup() { observers_ = new std::list<Observer*>(); }
  void attach(Observer* observer) override { observers_->push_back(observer); }

  void detach(Observer* observer) override {
    if (observers_->size() > 0) {
      observers_->remove(observer);
    }
  }

  void notify(const std::string& msg) override {
    std::cout << "msg: " << msg << std::endl;
    for (auto observer : *observers_) {
      observer->update(msg);
    }
  }

 private:
  std::list<Observer*>* observers_;
};

class RoomMate : public Observer {
 public:
  RoomMate(const std::string& name, const std::string& action,
           const std::string& now)
      : name_(name), action_(action), now_(now) {}

  void update(const std::string& msg) override {
    std::cout << "Name: " << name_ << std::endl;
    if (msg == "check") {
      std::cout << "Action: " << now_ << std::endl;
    } else {
      std::cout << "Action: " << action_ << std::endl;
    }
  }

  std::string getName() override { return name_; }

 private:
  std::string name_;
  std::string action_;
  std::string now_;
};
};  // namespace tf

//测试代码
int main() {
  using namespace tf;
  RoomMate* B =
      new RoomMate("B", "sleeping", "get dressed and run to classroom");
  RoomMate* C =
      new RoomMate("C", "playing games", "pay the fee and run to classroom");
  RoomMate* D = new RoomMate(
      "D", "shopping with girl friend",
      "go back to school and be worried about girl friend's angry");

  QQGroup* qqgroup = new QQGroup();
  qqgroup->attach(B);
  qqgroup->attach(C);
  qqgroup->attach(D);

  qqgroup->notify("check");
  qqgroup->notify("test");
  return 0;
}