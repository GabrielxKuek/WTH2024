import React, { useState } from 'react';
import { Trophy, XCircle, ListTodo, PenSquare, Check, X, Trash2 } from 'lucide-react';
import '../styles/Dashboard.css';

const Dashboard = () => {
  const [stats, setStats] = useState({
    wins: 15,
    losses: 5,
    tasksCompleted: 45,
    currentStreak: 7
  });

  const [tasks, setTasks] = useState([
    { id: 1, task: "Feed pet", timestamp: "2024-12-22 09:00", completed: true },
    { id: 2, task: "Play mini-game", timestamp: "2024-12-22 10:30", completed: true },
    { id: 3, task: "Clean pet house", timestamp: "2024-12-22 14:00", completed: false }
  ]);

  const [editingTask, setEditingTask] = useState(null);
  const [editValue, setEditValue] = useState("");
  const [newTask, setNewTask] = useState("");

  // Add new task
  const handleAddTask = (e) => {
    e.preventDefault();
    if (!newTask.trim()) return;

    const now = new Date();
    const timestamp = now.toLocaleString('en-US', {
      year: 'numeric',
      month: '2-digit',
      day: '2-digit',
      hour: '2-digit',
      minute: '2-digit',
      hour12: false
    });

    const newTaskItem = {
      id: Date.now(),
      task: newTask.trim(),
      timestamp: timestamp,
      completed: false
    };

    setTasks([newTaskItem, ...tasks]);
    setNewTask("");
  };

  // Edit task
  const handleEdit = (task) => {
    setEditingTask(task.id);
    setEditValue(task.task);
  };

  const handleSave = (id) => {
    setTasks(tasks.map(task => 
      task.id === id ? { ...task, task: editValue } : task
    ));
    setEditingTask(null);
  };

  // Delete task
  const handleDeleteTask = (id) => {
    setTasks(tasks.filter(task => task.id !== id));
  };

  // Toggle task completion
  const toggleTaskCompletion = (id) => {
    setTasks(tasks.map(task =>
      task.id === id ? { ...task, completed: !task.completed } : task
    ));
  };

  return (
    <div className="dashboard-container">
      <div className="content-container">
        {/* Header */}
        <div className="dashboard-header">
          <h1 className="dashboard-title">Virtual Pet Dashboard</h1>
          <p className="dashboard-subtitle">Track your pet's progress and activities</p>
        </div>
        
        {/* Stats Grid */}
        <div className="stats-grid">
          {/* Wins Card */}
          <div className="stats-card">
            <div className="stats-header">
              <h3 className="stats-title">Wins</h3>
              <div className="stats-icon-container wins">
                <Trophy size={20} className="text-green-600" />
              </div>
            </div>
            <p className="stats-value">{stats.wins}</p>
            <div className="card-accent wins"></div>
          </div>

          {/* Losses Card */}
          <div className="stats-card">
            <div className="stats-header">
              <h3 className="stats-title">Losses</h3>
              <div className="stats-icon-container losses">
                <XCircle size={20} className="text-red-600" />
              </div>
            </div>
            <p className="stats-value">{stats.losses}</p>
            <div className="card-accent losses"></div>
          </div>

          {/* Tasks Completed Card */}
          <div className="stats-card">
            <div className="stats-header">
              <h3 className="stats-title">Tasks Completed</h3>
              <div className="stats-icon-container tasks">
                <ListTodo size={20} className="text-blue-600" />
              </div>
            </div>
            <p className="stats-value">{stats.tasksCompleted}</p>
            <div className="card-accent tasks"></div>
          </div>

          {/* Current Streak Card */}
          <div className="stats-card">
            <div className="stats-header">
              <h3 className="stats-title">Current Streak</h3>
              <div className="stats-icon-container streak">
                <Trophy size={20} className="text-yellow-600" />
              </div>
            </div>
            <p className="stats-value">{stats.currentStreak} days</p>
            <div className="card-accent streak"></div>
          </div>
        </div>

        {/* Task History */}
        <div className="task-history">
          <div className="task-history-header">
            <h2 className="task-history-title">Task History</h2>
          </div>

          {/* Add Task Form */}
          <div className="task-form-container">
            <form onSubmit={handleAddTask} className="task-form">
              <input
                type="text"
                value={newTask}
                onChange={(e) => setNewTask(e.target.value)}
                placeholder="Add a new task..."
                className="task-input"
                maxLength={50}
              />
              <button type="submit" className="add-task-button">
                Add Task
              </button>
            </form>
          </div>

          {/* Task List */}
          <div className="task-list">
            {tasks.map(task => (
              <div key={task.id} className="task-item">
                {editingTask === task.id ? (
                  <input
                    type="text"
                    value={editValue}
                    onChange={(e) => setEditValue(e.target.value)}
                    className="task-input"
                  />
                ) : (
                  <div className="task-content">
                    <div className="flex items-center gap-3">
                      <span className={`task-text ${task.completed ? 'completed' : ''}`}>
                        {task.task}
                      </span>
                      <span className="timestamp-badge">
                        {task.timestamp}
                      </span>
                    </div>
                    
                    <div className="action-buttons">
                      {editingTask === task.id ? (
                        <button
                          onClick={() => handleSave(task.id)}
                          className="action-button"
                        >
                          <Check size={18} className="text-green-600" />
                        </button>
                      ) : (
                        <>
                          <button
                            onClick={() => handleEdit(task)}
                            className="action-button"
                          >
                            <PenSquare size={18} className="text-blue-600" />
                          </button>
                          <button
                            onClick={() => toggleTaskCompletion(task.id)}
                            className="action-button"
                          >
                            {task.completed ? (
                              <Check size={18} className="text-green-600" />
                            ) : (
                              <X size={18} className="text-gray-400" />
                            )}
                          </button>
                          <button
                            onClick={() => handleDeleteTask(task.id)}
                            className="action-button"
                          >
                            <Trash2 size={18} className="text-red-600" />
                          </button>
                        </>
                      )}
                    </div>
                  </div>
                )}
              </div>
            ))}
          </div>
        </div>
      </div>
    </div>
  );
};

export default Dashboard;

