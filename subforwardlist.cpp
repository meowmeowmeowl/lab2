#include <iostream>
#include <random>
#include <chrono>

// код здесь

// Чтобы сделать list более похожим на класс, давайте инкапсулировать указатель на начальный элемент

template<typename T>
class subforwardlist {

    // структура, описывающая узел списка (раньше указатель на неё был самим списком)
    struct Node {
        T data;
        Node* next;
        // сюда можете написать что угодно для вашего удобства, нет органичений по списку методов
    };


public:
    Node* begin_;
    Node* end_;
    unsigned int size_;
    //конструктор
    subforwardlist() : begin_(nullptr), end_(nullptr), size_(0){}
    //деструктор
    ~subforwardlist() {
        Node* x = begin_;
        while (x != nullptr) {
            Node* ne = x->next;
            delete x;
            x = ne;
        }
        begin_ = nullptr;
        end_ = nullptr;
        size_ = 0;
    }
    //explicit
    explicit subforwardlist(int c) :subforwardlist() {
        for (int i = 0; i < c; i++) {
            push_back(T{});
        }
 
    }
    //копирование
    subforwardlist(const subforwardlist& rhs) : subforwardlist() {
        Node* copy = rhs.begin_;
        while (copy != rhs.end_) {
            push_back(copy->data);
            copy = copy->next;
        }
    }
    //присваивание перемещением
    subforwardlist& operator = (subforwardlist&& rhs) noexcept {
        if (this == &rhs) {
            return *this;
        }
        end_ = begin_;
        size_ = 0;
        swap(rhs);
        return *this;
    }
    //перемещение
    subforwardlist(subforwardlist&& rhs) :subforwardlist() {
        swap(rhs);
    }
    //присваивание копированием
    subforwardlist& operator = (subforwardlist& rhs) {
        if (this == &rhs) {
            return *this;
        }
        end_ = begin_;
        size_ = 0;
        Node* copy = rhs.begin_;
        while (copy != rhs.end_) {
            push_back(copy->data);
            copy = copy->next;
        }
        return *this;
    }
    void swap(subforwardlist& rhs) noexcept {
        std::swap(begin_, rhs.begin_);
        std::swap(end_, rhs.end_);
        std::swap(size_, rhs.size_);
    }
    // далее описание методов (и только их!)
    void push_back(const T& data) {
        size_++;
        if (end_ != nullptr) {
            if (end_->next != nullptr) {
                end_->data = data;
                end_ = end_->next;
                return;
            }
            end_->next = new Node{ T{}, nullptr };
            end_->data = data;
            end_ = end_->next;
            return;
        }
        Node* newnode = new Node{ data, nullptr };
        Node* newend = new Node{ T{}, nullptr };
        newnode->next = newend;
        begin_= newnode;
        end_ = newend;
    } // добавление элемента в конец
    T pop_back() {
        if (size_ == 0) {
            return T{};
        }
        size_--;
        Node* copy = begin_;
        if (size_==0) {
            T x = copy->data;
            copy->data = T{};
            end_ = copy;
            return x;
        }
        while (copy->next->next != end_) {
            copy = copy->next;
        }
        T x = copy->next->data;
        copy->next->data = T{};
        end_ = copy->next;
        copy->next = end_;
        return x;
        std::cout << "popbacK";
    }  // удаление элемента с конца (если пустой -- возвращать T{} (default конструирование объекта типа T))
    void push_forward(const T& data) {
        if (size_ == 0) {
            push_back(data);
            return;
        }
        size_++;
        Node* newnode = new Node{ data, begin_ };
        begin_ = newnode;
    }//добавление элемента в начало недосписка
    T pop_forward() {
        if (size_ == 0) {
            return T{};
        }
        size_--;
        T x = begin_->data;
        Node* copy = begin_;
        begin_ = begin_->next;
        delete copy;
        return x;
    }


        //удаление элемента из начала недосписка (если пустой -- возвращать T{} (default конструирование объекта типа T))
        void push_where(unsigned int where, const T& data) {
        if (where > size_) {
            where = size_;
        }
        if (where == 0) {
            push_forward(data);
            return;
        }
        Node* newnode = new Node{ data, nullptr };
        Node* copy = begin_;
        for (unsigned int i = 0; i < where - 1; i++) {
            copy = copy->next;
        }
        newnode->next = copy->next;
        copy->next = newnode;
        size_++;
    }//добавление элемента с порядковым номером where
    T erase_where(unsigned int where) {
        if (size_ == 0 || where >= size_) {
            return T{};
        }
        if (where == 0) {
            return pop_forward();
        }
        Node* copy = begin_;
        for (unsigned int i = 0; i < where - 1;i++) {
            copy = copy->next;
        }
        Node* newnode = copy->next;
        T data = newnode->data;
        copy->next = newnode->next;
        delete newnode;
        newnode = nullptr;
        size_--;
        return data;
    }//удаление элемента с порядковым номером where (если пустой -- возвращать T{} (default конструирование объекта типа T))
    unsigned int size() const {
        return size_;
    }//определить размер недосписка
};
using list = subforwardlist<int>;
using std::cout;
using std::endl;
double get_time()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() / 1e6;
}
int rand_uns(int min, int max)
{
    unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
    static std::default_random_engine e(seed);
    std::uniform_int_distribution<int> d(min, max);
    return d(e);
}
int main()
{
    unsigned int n = 10000;
    int* test_sequence = new int[n], sum_for_O3 = 0, test_sequence_sum = 0, ongoing_sum = 0,
        * push_sequence = new int[n],
        * pop_sequence = new int[n],
        * pop_push_sequence_eq = new int[n],
        * pop_push_sequence_push = new int[n],
        * pop_push_sequence_pushpush = new int[n],
        * four_ways_test = new int[n];
    double start = 0, finish = 0, total = 0;
    cout << std::fixed;
    cout.precision(4);
    //----------- Initialization
    start = get_time();
    for (unsigned int i = 0; i < n; i++)
    {
        test_sequence[i] = rand_uns(0, n - 1);
        test_sequence_sum += test_sequence[i];
        push_sequence[i] = rand_uns(0, n - 1);
        pop_sequence[i] = rand_uns(0, n - 1);
        pop_push_sequence_eq[i] = rand_uns(0, 1);
        pop_push_sequence_push[i] = rand_uns(0, 5);
        pop_push_sequence_pushpush[i] = rand_uns(0, 10);
        four_ways_test[i] = rand_uns(0, 3);
    }

    finish = get_time();
    cout << "Test sequence initialization: \t\t\t\t" << finish - start << endl;
    list sv{};
    //----------- Test 000 Straight push_back
    start = get_time();
    for (unsigned int i = 0; i < n; i++)
    {
        sv.push_back(test_sequence[i]);
    }
    finish = get_time();
    cout << "000 Straight push_back: \t\t\t\t" << finish - start << endl;
    total += finish - start;
    //----------- Test 001 Straight pop_forward
    start = get_time();
    for (unsigned int i = 0; i < n; i++)
    {
        if (sv.pop_forward() != test_sequence[i])
        {
            cout << endl << "--- !!! Failed push/pop consistency !!! ---" << endl;
            return 0;
        }
    }
    finish = get_time();
    if (sv.size())
    {
        cout << endl << "--- !!! Failed push/pop consistency, some elememts stuck !!! ---" << endl;
        return 0;
    }
    cout << "001 Straight pop_forward: \t\t\t\t" << finish - start << endl;
    total += finish - start;
    //----------- Test 002 Straight push_forward
    start = get_time();
    for (unsigned int i = 0; i < n; i++)
    {
        sv.push_forward(test_sequence[i]);
    }
    finish = get_time();
    cout << "002 Straight push_forward: \t\t\t\t" << finish - start << endl;
    total += finish - start;
    //----------- Test 003 Straight pop_back
    start = get_time();
    for (unsigned int i = 0; i < n; i++)
    {
        if (sv.pop_back() != test_sequence[i])
        {
            cout << endl << "--- !!! Failed push/pop consistency !!! ---" << endl;
            return 0;
        }
    }
    finish = get_time();
    if (sv.size())
    {
        cout << endl << "--- !!! Failed push/pop consistency, some elememts stuck !!! ---" << endl;
        return 0;
    }
    cout << "003 Straight pop_back: \t\t\t\t\t" << finish - start << endl;
    total += finish - start;
    //----------- Test 004 Random push_where
    for (unsigned int i = 0; i < n; i++)
    {
        sv.push_back(test_sequence[i]);
    }
    start = get_time();
    for (unsigned int i = 0; i < n; i++)
    {
        sv.push_where(push_sequence[i], test_sequence[i]);
    }
    finish = get_time();
    if (sv.size() != 2 * n)
    {
        cout << endl << "--- !!! Failed push/pop consistency, wrong elements number !!! ---" << endl;
        return 0;
    }
    ongoing_sum = 0;
    for (unsigned int i = 0; i < 2 * n; i++)
    {
        ongoing_sum += sv.pop_back();
    }
    if (sv.size())
    {
        cout << endl << "--- !!! Failed push/pop consistency, some elememts stuck !!! ---" << endl;
        return 0;
    }
    if (ongoing_sum != 2 * test_sequence_sum)
    {
        cout << endl << "--- !!! Failed push/pop consistency, sum incoherent !!! ---" << endl;
        return 0;
    }
    cout << "004 Random push_where: \t\t\t\t\t" << finish - start << endl;
    total += finish - start;
    //----------- Test 005 Random erase_where
    for (unsigned int i = 0; i < 2 * n; i++)
    {
        sv.push_back(test_sequence[i % n]);
    }
    sum_for_O3 = 0;
    start = get_time();
    for (unsigned int i = 0; i < n; i++)
    {
        sum_for_O3 += sv.erase_where(pop_sequence[i]);
    }
    finish = get_time();
    if (sv.size() != n)
    {
        cout << endl << "--- !!! Failed push/pop consistency, wrong elements number !!! ---" << endl;
        return 0;
    }
    cout << "005 Random erase_where: \t\t\t\t" << finish - start << " \t\t" << sum_for_O3 << endl;
    total += finish - start;

    //----------- Test 006 Random pop/push back equal amount
    sum_for_O3 = 0; // This variable will be printed so O3 won't cut the whole thing.
    start = get_time();
    for (unsigned int i = 0; i < n; i++)
    {
        if (pop_push_sequence_eq[i])
            sv.push_back(test_sequence[i]);
        else
            sum_for_O3 += sv.pop_back();
    }
    finish = get_time();
    sv = list{};
    if (sv.size()) std::cout << "Move assign works wrongly!!!" << std::endl;
    cout << "006 Random pop/push back equal amount: \t\t\t" << finish - start << "\t\t" << sum_for_O3 << endl;
    total += finish - start;
    //----------- Test 007 Random pop/push back more push
    sum_for_O3 = 0; // This variable will be printed so O3 won't cut the whole thing.
    start = get_time();
    for (unsigned int i = 0; i < n; i++)
    {
        if (pop_push_sequence_push[i])
            sv.push_back(test_sequence[i]);
        else
            sum_for_O3 += sv.pop_back();
    }
    finish = get_time();
    sv = list{};
    cout << "007 Random pop/push back more push: \t\t\t" << finish - start << "\t\t" << sum_for_O3 << endl;
    total += finish - start;
    //----------- Test 008 Random pop/push back much more push
    sum_for_O3 = 0; // This variable will be printed so O3 won't cut the whole thing.
    start = get_time();
    for (unsigned int i = 0; i < n; i++)
    {
        if (pop_push_sequence_pushpush[i])
            sv.push_back(test_sequence[i]);
        else
            sum_for_O3 += sv.pop_back();
    }
    finish = get_time();

    sv = list{};
    cout << "008 Random pop/push back much more push: \t\t" << finish - start << "\t\t" << sum_for_O3 << endl;
    total += finish - start;
    //----------- Test 009 Random pop/push forward equal amount
    sum_for_O3 = 0; // This variable will be printed so O3 won't cut the whole thing.
    start = get_time();
    for (unsigned int i = 0; i < n; i++)
    {
        if (pop_push_sequence_eq[i])
            sv.push_forward(test_sequence[i]);
        else
            sum_for_O3 += sv.pop_forward();
    }
    finish = get_time();
    sv = list{};
    cout << "009 Random pop/push forward equal amount: \t\t" << finish - start << "\t\t" << sum_for_O3 << endl;
    total += finish - start;
    //----------- Test 010 Random pop/push forward more push
    sum_for_O3 = 0; // This variable will be printed so O3 won't cut the whole thing.
    start = get_time();
    for (unsigned int i = 0; i < n; i++)
    {
        if (pop_push_sequence_push[i])
            sv.push_forward(test_sequence[i]);
        else
            sum_for_O3 += sv.pop_forward();
    }
    finish = get_time();
    sv = list{};
    cout << "010 Random pop/push forward more push: \t\t\t" << finish - start << "\t\t" << sum_for_O3 << endl;
    total += finish - start;
    //----------- Test 011 Random pop/push forward much more push
    sum_for_O3 = 0; // This variable will be printed so O3 won't cut the whole thing.
    start = get_time();
    for (unsigned int i = 0; i < n; i++)
    {
        if (pop_push_sequence_pushpush[i])
            sv.push_forward(test_sequence[i]);
        else
            sum_for_O3 += sv.pop_forward();
    }
    finish = get_time();
    sv = list{};
    cout << "011 Random pop/push forward much more push: \t\t" << finish - start << "\t\t" << sum_for_O3 << endl;
    total += finish - start;
    //----------- Test 012 Random pop/push four ways
    sum_for_O3 = 0; // This variable will be printed so O3 won't cut the whole thing.
    start = get_time();
    for (unsigned int i = 0; i < n; i++)
    {
        if (four_ways_test[i] == 0)
            sv.push_back(test_sequence[i]);
        else if (four_ways_test[i] == 1)
            sum_for_O3 += sv.pop_back();
        else if (four_ways_test[i] == 2)
            sv.push_forward(test_sequence[i]);
        else
            sum_for_O3 += sv.pop_forward();
    }
    finish = get_time();
    sv = list{};
    cout << "012 Random pop/push four ways: \t\t\t\t" << finish - start << "\t\t" << sum_for_O3 << endl;
    total += finish - start;
    //----------- End of tests
    //
    cout << "-----------" << endl << "Alltests finished, total time: \t" << total << endl;
    delete[] test_sequence;
    delete[] pop_push_sequence_eq;
    delete[] pop_push_sequence_push;
    delete[] pop_push_sequence_pushpush;
    delete[] push_sequence,
        delete[] pop_sequence,
        delete[] four_ways_test;

    return 0;
}