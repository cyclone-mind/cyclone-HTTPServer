<template>
  <div class="py-12 bg-gray-50" id="results-section">
    <div class="max-w-3xl mx-auto px-4 sm:px-6 lg:px-8">
      <div class="bg-white shadow rounded-lg overflow-hidden">
        <div class="p-6">
          <div class="text-center mb-8">
            <div class="mx-auto flex items-center justify-center h-24 w-24 rounded-full bg-purple-100 mb-4">
              <svg class="h-12 w-12 text-purple-600" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M5 3v4M3 5h4M6 17v4m-2-2h4m5-16l2.286 6.857L21 12l-5.714 2.143L13 21l-2.286-6.857L5 12l5.714-2.143L13 3z" />
              </svg>
            </div>
            <h2 class="text-2xl font-bold text-gray-900 mb-2">测验完成！</h2>
            <p class="text-gray-600">你在游戏梗中获得了 <span class="font-bold text-purple-600">{{ results.score }}/{{ results.total_questions * 10 }}</span> 分</p>
          </div>
          <div class="mb-8">
            <div class="flex items-center justify-center mb-6">
              <div class="relative">
                <svg class="w-32 h-32" viewBox="0 0 36 36">
                  <path d="M18 2.0845
                      a 15.9155 15.9155 0 0 1 0 31.831
                      a 15.9155 15.9155 0 0 1 0 -31.831"
                      fill="none"
                      stroke="#e5e7eb"
                      stroke-width="3"
                      stroke-dasharray="100, 100"
                  />
                  <path d="M18 2.0845
                      a 15.9155 15.9155 0 0 1 0 31.831
                      a 15.9155 15.9155 0 0 1 0 -31.831"
                      fill="none"
                      stroke="#8b5cf6"
                      stroke-width="3"
                      :stroke-dasharray="`${percentage}, 100`"
                      class="progress-ring__circle"
                  />
                </svg>
                <div class="absolute inset-0 flex items-center justify-center">
                  <span class="text-3xl font-bold text-gray-900">{{ percentage }}%</span>
                </div>
              </div>
            </div>
            <div class="grid grid-cols-3 gap-4 text-center">
              <div>
                <p class="text-sm text-gray-500">正确</p>
                <p class="text-lg font-bold text-green-600">{{ results.correct_answers }}</p>
              </div>
              <div>
                <p class="text-sm text-gray-500">错误</p>
                <p class="text-lg font-bold text-red-600">{{ results.total_questions - results.correct_answers }}</p>
              </div>
              <div>
                <p class="text-sm text-gray-500">跳过</p>
                <p class="text-lg font-bold text-yellow-600">0</p>
              </div>
            </div>
          </div>
          <div class="mb-8">
            <div class="flex items-center justify-center mb-4">
              <div class="title-badge bg-gradient-to-r from-purple-500 to-pink-500 text-white px-4 py-2 rounded-full shadow-sm">
                <span class="font-medium">{{ results.title || 'Meme Scholar' }}</span>
              </div>
            </div>
            <p class="text-center text-gray-600">You've unlocked a new title! Keep quizzing to earn more.</p>
          </div>
          <div class="grid grid-cols-2 gap-4">
            <button class="inline-flex items-center justify-center px-4 py-2 border border-gray-300 shadow-sm text-sm font-medium rounded-md text-gray-700 bg-white hover:bg-gray-50">
              <i data-feather="list" class="mr-2"></i> 查看答案
            </button>
            <button class="inline-flex items-center justify-center px-4 py-2 border border-transparent text-sm font-medium rounded-md shadow-sm text-white bg-purple-600 hover:bg-purple-700">
              <i data-feather="award" class="mr-2"></i> 查看排行榜
            </button>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { computed } from 'vue';

const props = defineProps({
  results: Object
});

const percentage = computed(() => {
  return Math.round((props.results.score / (props.results.total_questions * 10)) * 100);
});
</script>

<style scoped>
.progress-ring__circle {
  transition: stroke-dashoffset 0.5s;
  transform: rotate(-90deg);
  transform-origin: 50% 50%;
}

.title-badge {
  position: relative;
  overflow: hidden;
}

.title-badge::after {
  content: '';
  position: absolute;
  top: -50%;
  left: -50%;
  width: 200%;
  height: 200%;
  background: linear-gradient(transparent, rgba(255,255,255,0.3), transparent);
  transform: rotate(30deg);
  animation: shine 3s infinite;
}

@keyframes shine {
  0% { left: -50%; }
  100% { left: 150%; }
}
</style>