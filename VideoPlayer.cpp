#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

class VideoPlayer {
private:
    cv::VideoCapture cap;
    cv::Mat currentFrame;
    std::string windowName;
    int totalFrames;
    int currentFrameNumber;
    double fps;
    
public:
    VideoPlayer() : windowName("Simple Video Player"), 
                         totalFrames(0), currentFrameNumber(0), fps(30.0) {}
    
    /**
     * Load video file and initialize player
     */
    bool loadVideo(const std::string& filename) {
        cap.open(filename);
        
        if (!cap.isOpened()) {
            std::cerr << "Error: Cannot open video file: " << filename << std::endl;
            return false;
        }
        
        // Get video properties
        totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
        fps = cap.get(cv::CAP_PROP_FPS);
        currentFrameNumber = 0;
        
        // Read first frame
        if (!cap.read(currentFrame)) {
            std::cerr << "Error: Cannot read first frame" << std::endl;
            return false;
        }
        
        std::cout << "Video loaded successfully:" << std::endl;
        std::cout << "  Total frames: " << totalFrames << std::endl;
        std::cout << "  FPS: " << fps << std::endl;
        std::cout << "  Resolution: " << currentFrame.cols << "x" << currentFrame.rows << std::endl;
        
        return true;
    }
    
    /**
     * Move to next frame
     */
    bool nextFrame() {
        if (currentFrameNumber >= totalFrames - 1) {
            return false; // At end of video
        }
        
        if (cap.read(currentFrame)) {
            currentFrameNumber++;
            return true;
        }
        return false;
    }
    
    /**
     * Move to previous frame
     */
    bool previousFrame() {
        if (currentFrameNumber <= 0) {
            return false; // At beginning of video
        }
        
        currentFrameNumber--;
        cap.set(cv::CAP_PROP_POS_FRAMES, currentFrameNumber);
        
        if (cap.read(currentFrame)) {
            return true;
        }
        return false;
    }
    
    /**
     * Jump to specific frame number
     */
    bool seekToFrame(int frameNumber) {
        if (frameNumber < 0 || frameNumber >= totalFrames) {
            return false;
        }
        
        cap.set(cv::CAP_PROP_POS_FRAMES, frameNumber);
        
        if (cap.read(currentFrame)) {
            currentFrameNumber = frameNumber;
            return true;
        }
        return false;
    }
    
    /**
     * Display current frame in window
     */
    void displayFrame() {
        if (!currentFrame.empty()) {
            // Add frame info overlay
            cv::Mat displayFrame = currentFrame.clone();
            std::string info = "Frame: " + std::to_string(currentFrameNumber + 1) + 
                              "/" + std::to_string(totalFrames);
            cv::putText(displayFrame, info, cv::Point(10, 30), 
                       cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
            
            cv::imshow(windowName, displayFrame);
        }
    }
    
    /**
     * Print current frame information
     */
    void printFrameInfo() {
        double progress = (double)(currentFrameNumber + 1) / totalFrames * 100.0;
        std::cout << "\rFrame: " << (currentFrameNumber + 1) << "/" << totalFrames 
                  << " (" << std::fixed << std::setprecision(1) << progress << "%)" 
                  << std::flush;
    }
    
    /**
     * Main playback loop with controls
     */
    void startPlayback() {
        if (currentFrame.empty()) {
            std::cerr << "No video loaded!" << std::endl;
            return;
        }
        
        cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
        
        // Show controls
        std::cout << "\n=== Simple Video Player Controls ===" << std::endl;
        std::cout << "SPACE    : Play/Pause" << std::endl;
        std::cout << "→ or D   : Next frame" << std::endl;
        std::cout << "← or A   : Previous frame" << std::endl;
        std::cout << "HOME     : Go to first frame" << std::endl;
        std::cout << "END      : Go to last frame" << std::endl;
        std::cout << "G        : Go to specific frame" << std::endl;
        std::cout << "ESC or Q : Quit" << std::endl;
        std::cout << "===================================\n" << std::endl;
        
        bool playing = false;
        bool quit = false;
        
        while (!quit) {
            displayFrame();
            printFrameInfo();
            
            // Wait for key input (33ms = ~30fps when playing, 0 = wait indefinitely when paused)
            int key = cv::waitKey(playing ? 33 : 0) & 0xFF;
            
            switch (key) {             
                case 'q':
                case 'Q':
                    quit = true;
                    break;
                    
                case ' ': // SPACE - Play/Pause
                    playing = !playing;
                    std::cout << "\n" << (playing ? "▶ Playing" : "⏸ Paused") << std::endl;
                    break;
                    
                case 'd':
                case 'D':
                    if (!nextFrame()) {
                        std::cout << "\nEnd of video reached" << std::endl;
                        playing = false;
                    }
                    break;
                    
                case 'a':
                case 'A':
                    if (!previousFrame()) {
                        std::cout << "\nBeginning of video reached" << std::endl;
                    }
                    break;
                    
                case 'h':
                case 'H':
                    seekToFrame(0);
                    std::cout << "\nJumped to first frame" << std::endl;
                    break;
                
                case 'e':
                case 'E':
                    seekToFrame(totalFrames - 1);
                    std::cout << "\nJumped to last frame" << std::endl;
                    break;
                    
                case 'g':
                case 'G': {
                    std::cout << "\nEnter frame number (1-" << totalFrames << "): ";
                    int targetFrame;
                    std::cin >> targetFrame;
                    
                    if (targetFrame >= 1 && targetFrame <= totalFrames) {
                        seekToFrame(targetFrame - 1); // Convert to 0-based
                        std::cout << "Jumped to frame " << targetFrame << std::endl;
                    } else {
                        std::cout << "Invalid frame number!" << std::endl;
                    }
                    break;
                }
                    
                default:
                    // Auto-advance when playing
                    if (playing) {
                        if (!nextFrame()) {
                            std::cout << "\nEnd of video reached" << std::endl;
                            playing = false;
                        }
                    }
                    break;
            }
        }
        
        cv::destroyAllWindows();
        std::cout << "\nPlayback stopped." << std::endl;
    }
    
    /**
     * Get current frame number (0-based)
     */
    int getCurrentFrame() const {
        return currentFrameNumber;
    }
    
    /**
     * Get total number of frames
     */
    int getTotalFrames() const {
        return totalFrames;
    }
    
    /**
     * Get video FPS
     */
    double getFPS() const {
        return fps;
    }
};

// Main function - simple usage example
int main(int argc, char* argv[]) {
    std::cout << "=== Video Player ===" << std::endl;
    std::cout << "Built with OpenCV \n" << std::endl;
    
    std::string videoFile;
    
    // Get video file path
    if (argc > 1) {
        videoFile = argv[1];
    } else {
        std::cout << "Enter video file path: ";
        std::getline(std::cin, videoFile);
    }
    
    // Create and use video player
    VideoPlayer player;
    
    if (player.loadVideo(videoFile)) {
        player.startPlayback();
    } else {
        std::cerr << "Failed to load video: " << videoFile << std::endl;
        return -1;
    }
    
    return 0;
}

