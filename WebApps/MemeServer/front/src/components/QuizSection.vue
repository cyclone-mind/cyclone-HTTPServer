<template>
  <div class="py-12 bg-gray-50" id="quiz-section">
    <div class="max-w-3xl mx-auto px-4 sm:px-6 lg:px-8">
      <div class="bg-white shadow rounded-lg overflow-hidden">
        <div class="p-6">
          <!-- Progress Bar -->
          <div class="mb-6">
            <div class="flex justify-between items-center mb-2">
              <span class="text-sm font-medium text-gray-700">问题 <span>{{ currentQuestionIndex + 1 }}</span>/10</span>
              <span class="text-sm font-medium text-purple-600">分数: 0</span>
            </div>
            <div class="w-full bg-gray-200 rounded-full h-2.5">
              <div class="bg-purple-600 h-2.5 rounded-full" :style="{ width: progressWidth }"></div>
            </div>
          </div>
          <!-- Question -->
          <div class="mb-8" v-if="currentQuestion">
            <h3 class="text-xl font-medium text-gray-900 mb-4">{{ currentQuestion.question_text }}</h3>
            <div class="grid grid-cols-1 gap-4">
              <div 
                v-for="(option, key) in currentQuestion.options" 
                :key="key"
                class="option-card border border-gray-200 rounded-lg p-4 cursor-pointer"
                :class="{ 'selected': selectedOption === key }"
                @click="selectOption(key)"
              >
                <div class="flex items-center">
                  <div class="flex-shrink-0 h-5 w-5 rounded-full border border-gray-300 flex items-center justify-center mr-3">
                    <span class="text-xs font-medium">{{ key }}</span>
                  </div>
                  <p class="text-gray-700">{{ option }}</p>
                </div>
              </div>
            </div>
          </div>
          <!-- Navigation -->
          <div class="flex justify-between">
            <button 
              class="inline-flex items-center px-4 py-2 border border-gray-300 shadow-sm text-sm font-medium rounded-md text-gray-700 bg-white hover:bg-gray-50"
              :disabled="currentQuestionIndex === 0"
            >
              <i data-feather="chevron-left" class="mr-2"></i> 上一页
            </button>
            <button 
              @click="$emit('next-question')"
              class="inline-flex items-center px-4 py-2 border border-transparent text-sm font-medium rounded-md shadow-sm text-white bg-purple-600 hover:bg-purple-700">
              下一页 <i data-feather="chevron-right" class="ml-2"></i>
            </button>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, computed, watch } from 'vue';

const props = defineProps({
  currentQuestions: Array,
  currentAnswers: Array
});

const emit = defineEmits(['submit-answer', 'next-question']);

const currentQuestionIndex = ref(0);
const selectedOption = ref(null);

const currentQuestion = computed(() => {
  return props.currentQuestions[currentQuestionIndex.value];
});

const progressWidth = computed(() => {
  return `${((currentQuestionIndex.value + 1) / 10) * 100}%`;
});

const selectOption = (option) => {
  selectedOption.value = option;
  emit('submit-answer', {
    question_id: currentQuestion.value.id,
    answer: option
  });
};

// 监听题目变化，重置选项
watch(() => props.currentQuestions, (newQuestions) => {
  if (newQuestions && newQuestions.length > 0) {
    currentQuestionIndex.value = 0;
    selectedOption.value = null;
  }
});
</script>

<style scoped>
.option-card {
  transition: all 0.2s ease;
}
.option-card:hover {
  transform: translateY(-2px);
  background-color: #f3f4f6;
}
.option-card.selected {
  background-color: #dbeafe;
  border-color: #3b82f6;
}
</style>