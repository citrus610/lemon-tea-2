#include "thread.h"

namespace LemonTea
{

Thread::Thread()
{
    this->clear();
};

Thread::~Thread()
{
    this->stop();
};

bool Thread::start(Board board, PieceType hold, std::vector<PieceType> queue, Bag bag, int b2b, int ren)
{
    if (this->thread != nullptr || this->flag_running.test()) {
        return false;
    }

    this->clear();

    this->search.init(board, hold, queue, bag, b2b, ren);

    this->flag_running.test_and_set();
    this->flag_advance.clear();
    this->flag_reset.clear();

    this->forecast = false;
    this->pre_layer = 0;
    this->new_layer = 1;
    this->index = 0;
    this->width = SEARCH_WIDTH;
    this->count = this->search.candidate.size();
    this->depth = 1;

    this->thread = new std::thread([&] ()
    {
        while (this->flag_running.test())
        {
            this->search.think(this->pre_layer, this->new_layer, this->index, this->width, this->forecast, this->count, this->depth);

            if (this->flag_advance.test()) {
                {
                    std::unique_lock<std::mutex> lk(mutex);
                    if (!this->buffer_advance.empty()) {
                        this->search.advance(this->buffer_advance[0].placement, this->buffer_advance[0].next);
                        this->buffer_advance.clear();
                        this->forecast = false;
                        this->pre_layer = 0;
                        this->new_layer = 1;
                        this->index = 0;
                        this->width = SEARCH_WIDTH;
                        this->count = this->search.candidate.size();
                        this->depth = 1;
                    }
                }
                this->flag_advance.clear();
                this->flag_advance.notify_one();
            }

            if (this->flag_reset.test()) {
                {
                    std::unique_lock<std::mutex> lk(mutex);
                    if (!this->buffer_reset.empty()) {
                        this->search.reset(this->buffer_reset[0].board, this->buffer_reset[0].b2b, this->buffer_reset[0].ren);
                        this->buffer_reset.clear();
                        this->forecast = false;
                        this->pre_layer = 0;
                        this->new_layer = 1;
                        this->index = 0;
                        this->width = SEARCH_WIDTH;
                        this->count = this->search.candidate.size();
                        this->depth = 1;
                    }
                }
                this->flag_reset.clear();
                this->flag_reset.notify_one();
            }
        }
    });

    return true;
};

bool Thread::stop()
{
    if (this->thread == nullptr || !this->flag_running.test()) {
        return false;
    }

    this->flag_running.clear();
    this->thread->join();
    delete this->thread;
    this->clear();

    return true;
};

bool Thread::advance(Piece placement, std::vector<PieceType> next)
{
    if (this->thread == nullptr || !this->flag_running.test()) {
        return false;
    }

    {
        std::unique_lock<std::mutex> lk(mutex);
        if (this->buffer_advance.empty()) {
            this->buffer_advance.push_back({placement, next});
        }
        else {
            this->buffer_advance[0] = {placement, next};
        }
    }
    this->flag_advance.test_and_set();
    // this->flag_advance.wait(true);

    return true;
};

bool Thread::reset(Board board, int b2b, int ren)
{
    if (this->thread == nullptr || !this->flag_running.test()) {
        return false;
    }

    {
        std::unique_lock<std::mutex> lk(mutex);
        if (this->buffer_reset.empty()) {
            this->buffer_reset.push_back({board, b2b, ren});
        }
        else {
            this->buffer_reset[0] = {board, b2b, ren};
        }
    }
    this->flag_reset.test_and_set();
    // this->flag_reset.wait(true);

    return true;
};

bool Thread::request(int incomming, Plan& plan)
{
    if (this->thread == nullptr || !this->flag_running.test()) {
        return false;
    }

    Candidate candidate;
    std::vector<Candidate> candidates;
    Node root;
    {
        std::unique_lock<std::mutex> lk(mutex);
        candidates = this->search.candidate;
        root = this->search.root;
        plan.node = this->count;
        plan.depth = this->depth;
    }

    double total = 0;
    for (int i = 0; i < int(candidates.size()); ++i) {
        total += double(candidates[i].visit);
    }

    Search::pick(root, candidates, incomming, candidate);
    plan.placement = candidate.placement;
    plan.root = root.state;
    plan.result = candidate.node.state;
    plan.eval = candidate.visit / total;

    return true;
};

void Thread::clear()
{
    this->flag_running.clear();
    this->flag_advance.clear();
    this->flag_reset.clear();

    this->thread = nullptr;

    this->forecast = false;
    this->pre_layer = 0;
    this->new_layer = 0;
    this->index = 0;
    this->width = 0;
    this->count = 0;
    this->depth = 0;

    this->search = Search();
};

};