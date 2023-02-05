
namespace FileUtils
{
    class IFileNotifier;
    enum class FileEvents : int;

    class INonBlockingFileNotifier
    {
    public:
        void add_target(const std::filesystem::path& targetFile) = 0;
        void remove_target(const std::filesystem::path& targetFile) = 0;

        start_watcher(std::function<void(const vector<file>& changedFiles)> notifierLambda)
        {
            //  Check if the watcher thread is already running.
            if (m_thread.joinable())
                return;

            auto myLocalLambda = [&](vector<int> readyToReadFiles) {
                for (auto file : readyToReadFiles)
                {
                    if (file == terminateEventFile)
                    {
                        terminate = true;
                        break;
                    }

                    read(m_notifyFd, eventList, sizeof(eventList));
                    //  parse through events list and prepare a vector<files changed>
                    notifierLambda(filesChangedList)
                }
            };

            threadFunc = [&]()
            {
                while (!terminate)
                {
                    m_nonBlockingObserver.start(myLocalLambda)
                }
            }

            m_thread = std::thread(threadFunc);
        }

        stop_watcher()
        {
            //  write to termination file which will unblock
            //  the m_nonBlockingObserver and it will return the
            //  terminate file handle in readyToReadFiles list.
            //  The lambda will set terminate to true and we'll
            //  break out of the loop in start_watching()

            if (!m_thread.joinable())
                m_thread.join();
        }

    private:
        I
    }
}