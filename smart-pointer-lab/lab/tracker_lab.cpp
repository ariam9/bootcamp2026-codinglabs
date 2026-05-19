// =============================================================================
// RAII & Smart Pointers — Staged Lab
// tracker_lab.cpp
//
// Work through each stage in order. Uncomment the stage's main() block when
// you reach it and comment out the previous one.
//
// Compile with:
//   g++ -std=c++20 -Wall -Wextra -fsanitize=address,undefined -g tracker_lab.cpp -o tracker_lab
// =============================================================================

#include <iostream>
#include <memory>
#include <vector>
#include <atomic>
#include <utility>
#include <stdexcept>

// ─────────────────────────────────────────────────────────────────────────────
// Utility: monotonically increasing ID so every Tracker has a unique name.
// ─────────────────────────────────────────────────────────────────────────────
static int next_id() {
    static std::atomic<int> counter{0};
    return ++counter;
}

// =============================================================================
// STAGE 1 — Implement Tracker<T> using a raw pointer.
//
// Your tasks:
//   1. Fill in the constructor:  allocate a T on the heap, assign id_, log "[id] born".
//   2. Fill in the destructor:   delete the pointer, log "[id] destroyed".
//   3. Fill in the copy constructor:  deep-copy, log "[new_id] copied from [src_id]".
//   4. Fill in copy assignment (operator=): deep-copy, handle self-assignment, log.
//   5. Implement get() to return a reference to the stored T.
//
// Rule of Three: if you write any one of {destructor, copy ctor, copy assign},
// you need to write all three. Here you'll write all three by hand.
// =============================================================================

// template <typename T>
// class Tracker {
// private:
//     T*  data_;  // owning raw pointer — YOU are responsible for deleting this
//
// public:
//     int id_;
//
//     explicit Tracker(T value) : id_(next_id()), data_(new T(value)) {
//         // TODO: assign id_ using next_id()
//         // TODO: allocate data_ with new
//         // TODO: log "[id_] born"
//         std::cerr << id_ << " born" << std::endl;
//     }
//
//     ~Tracker() {
//         // TODO: log "[id_] destroyed"
//         // TODO: delete data_
//         std::cerr << id_ << " destroyed" << std::endl;
//         if (data_) delete data_;
//     }
//
//     Tracker(const Tracker& other) : id_(next_id()), data_(new T(*other.data_)) {
//         // TODO: assign id_ using next_id()
//         // TODO: deep-copy other.data_
//         // TODO: log "[id_] copied from [other.id_]"
//         std::cerr << id_ << " copied from " << other.id_ << std::endl;
//     }
//
//     Tracker& operator=(const Tracker& other) {
//         // TODO: guard against self-assignment (if (this == &other) return *this;)
//         // TODO: delete old data_
//         // TODO: deep-copy other.data
//         // TODO: log "[id_] assigned from [other.id_]"
//         if (this == &other) return *this;
//         delete data_;
//         data_ = new T(*other.data_);
//         id_ = other.id_;
//         std::cerr << id_ << " copy assigned from " << other.id_ << std::endl;
//         return *this;
//     }
//
//     // implement getters
//     // (do you need multiple? think about const correctness)
//     T& get() {return *data_;}
//
//     const T& get() const {return *data_;}
// };
//
// // ─── Stage 1 main ─────────────────────────────────────────────────────────────
// // Uncomment this block for Stage 1. Comment it out before Stage 2.
//
// int main() {
//     std::cerr << "=== Stage 1: raw pointer Tracker ===\n";
//
//     // Predict the log before running:
//     //   How many "born" lines?  How many "destroyed" lines?
//     {
//         Tracker<int> a(10);
//         Tracker<int> b(20);
//         Tracker<int> c = a;   // copy constructor
//
//         std::cerr << "a=" << a.get() << " b=" << b.get() << " c=" << c.get() << "\n";
//     } // a, b, c destroyed here
//
//     std::cerr << "\n--- vector reallocation test ---\n";
//     // Watch what happens when the vector resizes: it copies elements.
//     // Count "born", "copied from", and "destroyed" lines.
//     {
//         std::vector<Tracker<int>> v;
//         v.push_back(Tracker<int>(1));
//         v.push_back(Tracker<int>(2));
//         v.push_back(Tracker<int>(3));
//         std::cerr << "vector holds " << v.size() << " trackers\n";
//     }
//
//     std::cerr << "\n--- self-assignment test ---\n";
//     {
//         Tracker<int> x(99);
//         x = x;  // self-assignment: what goes wrong if you don't guard?
//         std::cerr << "x=" << x.get() << "\n";
//     }
//
//     return 0;
// }


// =============================================================================
// STAGE 2 — Refactor to use std::unique_ptr<T> as the member.
//
// Your tasks:
//   1. Change data_ from T* to std::unique_ptr<T>.
//   2. DELETE the destructor (unique_ptr handles it — Rule of Zero).
//   3. DELETE the copy constructor and copy assignment (unique_ptr is move-only).
//   4. ADD a move constructor:   transfer id_, move data_, log "[id_] moved from [src_id]".
//   5. ADD move assignment:      same idea.
//   6. Re-run the vector test. You'll need std::move — why?
//
// Key insight: by using a move-only member, the class *becomes* move-only,
// which is correct — exclusive ownership means no copies.
// =============================================================================

template <typename T>
class Tracker {
private:
    std::unique_ptr<T>  data_;  // owning raw pointer — YOU are responsible for deleting this

public:
    int id_;

    explicit Tracker(T value) : id_(next_id()), data_(std::make_unique<T>(value)) {
        std::cerr << id_ << " born" << std::endl;
    }

    ~Tracker() {
        std::cerr << id_ << " destroyed" << std::endl;
    }

    Tracker(const Tracker& other) = delete;
    Tracker& operator=(const Tracker& other) = delete;

    Tracker(Tracker&& other) : id_(next_id()), data_(std::move(other.data_)) {
        std::cerr << id_ << " moved from " << other.id_;
    }

    Tracker& operator=(Tracker&& other) {
        if (this == &other) return *this;
        data_ = std::move(other.data_);
        std::cerr << id_ << " move assigned from " << other.id_ << std::endl;
        return *this;
    }

    // implement getters
    // (do you need multiple? think about const correctness)
    T& get() {return *data_;}

    const T& get() const {return *data_;}
};

// ─── Stage 2 main ─────────────────────────────────────────────────────────────

// int main() {
//     std::cerr << "=== Stage 2: unique_ptr Tracker ===\n";

//     {
//         Tracker<int> a(10);
//         // Tracker<int> b = a;           // should not compile — why?
//         Tracker<int> b = std::move(a);  // move is fine
//         std::cerr << "b=" << b.get() << "\n";
//         // What is a's state here? Try printing a.get() and see what happens.
//     }

//     std::cerr << "\n--- vector with move-only elements ---\n";
//     {
//         std::vector<Tracker<int>> v;
//         v.push_back(Tracker<int>(1));
//         v.push_back(Tracker<int>(2));
//         v.push_back(Tracker<int>(3));
//         // Notice: the vector now moves elements instead of copying.
//         // The log should be shorter than Stage 1's.
//     }

//     // Factory function pattern — common in real codebases:
//     auto make = [](int val) -> std::unique_ptr<Tracker<int>> {
//         return std::make_unique<Tracker<int>>(val);
//     };
//     auto t = make(42);
    // std::cerr << "factory-made tracker: " << t->get() << "\n";

//     return 0;
// }


// =============================================================================
// STAGE 3 — Shared ownership with std::shared_ptr<T>.
//
// Introduce Portfolio: a container that holds shared_ptr<Tracker<int>>.
// Two portfolios can share the same Tracker.
//
// Your tasks:
//   1. Implement Portfolio with a vector<shared_ptr<Tracker<int>>> member.
//   2. Add Portfolio::add(shared_ptr<Tracker<int>>) and Portfolio::print().
//   3. Write make_tracker(int) returning a shared_ptr<Tracker<int>> via make_shared.
//   4. Add the same shared_ptr to two portfolios.
//   5. Print use_count() after each addition to watch the ref count climb.
//   6. Let one portfolio go out of scope — watch use_count() drop.
// =============================================================================
class Portfolio {
public:
    std::string name_;
    std::vector<std::shared_ptr<Tracker<int>>> holdings_;

    explicit Portfolio(std::string name) : name_(std::move(name)) {}

    void add(std::shared_ptr<Tracker<int>> t) {
        // TODO: push_back t, then log name_ + " now holds tracker " + id
        //       and print t.use_count()
        holdings_.push_back(t);
        std::cerr << name_ << " now holds tracker " << t->id_ << std::endl;
        std::cerr << "use count of shared_ptr holding tracker " << t->id_ << ": " << t.use_count() << std::endl;
    }

    void print() const {
        // TODO: implement print out for each holding in the portfolio
        std::cerr << "portfolio " << name_ << " holds: ";
        for (auto holding : holdings_) std::cerr << holding->get() << " ";
        std::cerr << std::endl;
    }
};

std::shared_ptr<Tracker<int>> make_tracker(int value) {
    // TODO: produce a shared pointer
    // Q1: Why make a make_tracker function?
    //to make shared pointers more easily instead of the full verbose expression with the template
    // Q2: There are two ways to make a shared_ptr<Tracker<int>>. Which one is more appropriate here and is one always better?

    //method 1: less efficient because it requires 2 allocations - one for the Tracker object and one for the shared_ptr control block
    //return std::shared_ptr<Tracker<int>>(new Tracker<int>(value));

    //method 2: more efficient because it requires only 1 allocation - for both the Tracker object and the control block together
    return std::make_shared<Tracker<int>>(value);
}

// ─── Stage 3 main ─────────────────────────────────────────────────────────────

// int main() {
//     std::cerr << "=== Stage 3: shared_ptr ===\n";
//
//     auto t1 = make_tracker(100);
//     auto t2 = make_tracker(200);
//
//     std::cerr << "After creation: t1.use_count=" << t1.use_count()
//               << "  t2.use_count=" << t2.use_count() << "\n";
//
//     {
//         Portfolio alpha("Alpha");
//         Portfolio beta("Beta");
//
//         alpha.add(t1);
//         alpha.add(t2);
//         beta.add(t1);   // t1 now shared between alpha and beta
//
//         alpha.print();
//         beta.print();
//
//         std::cerr << "t1.use_count=" << t1.use_count() << "\n";  // should be 3
//     }
//     // alpha and beta destroyed here — their copies of the shared_ptr drop
//     std::cerr << "After portfolios destroyed: t1.use_count=" << t1.use_count() << "\n"; // should be 1
//
//     return 0;
// }

// =============================================================================
// STAGE 4 — Cycles: the one hole in shared_ptr.
//
// See cycle.cpp for the deliberately broken snippet.
// Come back here once you understand and have fixed that file.
//
// Discussion questions (answer before moving to Stage 5):
//   Q1: Which direction of a link should be weak? Why?
//in the case of the node example, the next_ variable should be a weak_ptr, because the local variables a and b have to be shared_ptrs, and we want to prevent cyclic references
//in the case of order and trader, the order should have a weak_ptr reference to the trader so that there aren't circular shared references and at the same time, active orders should remain in memory, so trader should hold a shared_ptr to order
//
//   Q2: How do you safely dereference a weak_ptr?
//you must first promote the weak_ptr temporarily to a shared_ptr using weak_ptr::lock, and then check if the returned shared_ptr is null(empty) or not, before deciding to dereference it
//
//   Q3: What happens to a weak_ptr when its shared_ptr owner is destroyed?
//the weak_ptr expires when its shared_ptr owner is destroyed, that is, weak_ptr::lock will return an empty shared_ptr(which is null basically), and weak_ptr::expired will return true, indicating that it has expired
//
//   Q4: Look at the Trader-Order set of objects. Why is the back-ref creation bad, and how do we fix it?
//the GIVEN back-ref creation is really bad, because it tries to assign the order's owner to the same order's owner(logically doesn't make sense at all)
//some things i thought of:
//using o->owner_ = std::shared_ptr(this); - this doesn't make sense since a new shared_ptr for this is created every time an order is placed, which will create a new control block every time, leading to multiple deletions/frees
//using o->owner_ = shared_from_this(); after making Trader inherit from std::enable_shared_from_this<Trader>, so that the shared_From_this function can return a shared_ptr to this, with a consistently same control block(as long as Trader is initially managed by a shared_ptr)
//using o->owner_ = weak_from_this(); works similarly as shared_from_this, but directly returns a weak_ptr(more efficient for my purpose) instead of having to create a shared_ptr, make a weak_ptr from it, and then destroy the shared_ptr
//
// =============================================================================

// =============================================================================
// STAGE 5 — See leak_hunt.cpp.
// =============================================================================

//─── Placeholder main (remove once you uncomment a stage's main) ──────────────
int main() {
    std::cerr << "Uncomment a stage's main() block and recompile.\n";
    return 0;
}
